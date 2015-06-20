<?hh
namespace Titon\Intl;

use Titon\Io\Reader\HackReader;
use Titon\Test\TestCase;
use Titon\Utility\Config;
use Titon\Utility\State\Cookie;
use Titon\Utility\State\Server;

/**
 * @property \Titon\Intl\Translator $object
 */
class TranslatorTest extends TestCase {

    protected function setUp(): void {
        parent::setUp();

        $this->loader = new MessageLoader(Vector {new HackReader()});
        $this->object = new Translator($this->loader);
    }

    public function testAddGetLocale(): void {
        $locale = new Locale('en');

        $this->object->addLocale($locale);

        $this->assertEquals($locale, $this->object->getLocale('en'));
    }

    public function testAddLocalePersistsInstance(): void {
        $locale1 = new Locale('en');
        $locale2 = new Locale('en');

        $this->object->addLocale($locale1);
        $this->object->addLocale($locale2);

        $this->assertSame($locale1, $this->object->getLocale('en'));
    }

    public function testAddLocaleStoresInRegistry(): void {
        $locale = new Locale('en');

        $this->object->addLocale($locale);

        $this->assertSame($locale, LocaleRegistry::factory('en'));
    }

    public function testAddLocaleAddsResourcePaths(): void {
        $this->object->addResourcePaths('foo', Set {'/foo'});

        $locale = new Locale('en');

        $this->object->addLocale($locale);

        $this->assertEquals(Map {
            'common' => Set {'/foo/locales/en/', SRC_DIR . '/Titon/Intl/locales/en/'}
        }, $locale->getLocaleBundle()->getPaths());

        $this->assertEquals(Map {
            'foo' => Set {'/foo/messages/en/'},
            'common' => Set {SRC_DIR . '/Titon/Intl/messages/en/'}
        }, $locale->getMessageBundle()->getPaths());
    }

    public function testAddLocaleAddsParent(): void {
        $this->object->addLocale(new Locale('en_US'));

        $this->assertInstanceOf('Titon\Intl\Locale', $this->object->getLocale('en_US'));
        $this->assertInstanceOf('Titon\Intl\Locale', $this->object->getLocale('en'));
    }

    public function testAddLocaleSetsFallbackIfNoneSet(): void {
        $locale = new Locale('en');

        $this->object->addLocale($locale);

        $this->assertSame($locale, $this->object->getFallback());
    }

    public function testAddGetPaths(): void {
        $this->object->addResourcePaths('foo', Set {'/foo', '/oof'});
        $this->object->addResourcePaths('bar', Set {'/bar'});

        $this->assertEquals(Set {
            SRC_DIR . '/Titon/Intl/locales',
            '/foo/locales',
            '/oof/locales',
            '/bar/locales'
        }, $this->object->getLocalePaths());

        $this->assertEquals(Map {
            'foo' => Set {'/foo/messages', '/oof/messages'},
            'bar' => Set {'/bar/messages'},
            'common' => Set {SRC_DIR . '/Titon/Intl/messages'}
        }, $this->object->getMessagePaths());
    }

    public function testCascade(): void {
        $en = new Locale('en_US');
        $fr = new Locale('fr');

        $this->object->addLocale($en);
        $this->object->addLocale($fr);
        $this->object->setFallback('fr');
        $this->object->localize('en_US');

        $this->assertEquals(Set {'en_US', 'en', 'fr'}, $this->object->cascade());
    }

    public function testDetectFromAcceptLangHeader(): void {
        $httpAccepts = [
            'en-us, en;q=0.5' => 'en_US',
            'fr ,en;q=0.8' => 'en',
            'de' => 'es' // Wont match and will use the fallback
        ];

        $this->object->addLocale(new Locale('en_US'));
        $this->object->addLocale(new Locale('es'));
        $this->object->setFallback('es');

        foreach ($httpAccepts as $httpAccept => $localeCode) {
            $_SERVER['HTTP_ACCEPT_LANGUAGE'] = $httpAccept;
            Server::initialize($_SERVER);

            $this->object->detect();
            $current = $this->object->current();

            $this->assertInstanceOf('Titon\Intl\Locale', $current);
            $this->assertEquals($localeCode, $current->getCode());
        }
    }

    public function testDetectFromCookie(): void {
        $cookies = [
            'en_US' => 'en_US',
            'de' => 'es' // Wont match and will use the fallback
        ];

        $this->object->addLocale(new Locale('en_US'));
        $this->object->addLocale(new Locale('es'));
        $this->object->setFallback('es');

        foreach ($cookies as $cookie => $localeCode) {
            $_COOKIE['locale'] = $cookie;
            Cookie::initialize($_COOKIE);

            $this->object->detect();
            $current = $this->object->current();

            $this->assertInstanceOf('Titon\Intl\Locale', $current);
            $this->assertEquals($localeCode, $current->getCode());
        }
    }

    public function testGetSetFallback(): void {
        $en = new Locale('en');
        $fr = new Locale('fr');

        $this->object->addLocale($en);

        $this->assertEquals($en, $this->object->getFallback());

        $this->object->addLocale($fr);

        $this->assertEquals($en, $this->object->getFallback());

        $this->object->setFallback('fr');

        $this->assertEquals($fr, $this->object->getFallback());
    }

    /**
     * @expectedException \Titon\Intl\Exception\MissingLocaleException
     */
    public function testGetLocaleThrowsMissingError(): void {
        $this->object->getLocale('en');
    }

    public function testGetLocales(): void {
        $en = new Locale('en');
        $fr = new Locale('fr');

        $this->object->addLocale($en);
        $this->object->addLocale($fr);

        $this->assertEquals(Map {
            'en' => $en,
            'fr' => $fr
        }, $this->object->getLocales());
    }

    public function testGetMessageLoader(): void {
        $this->assertEquals($this->loader, $this->object->getMessageLoader());
    }

    public function testIs(): void {
        $this->assertFalse($this->object->is('en_US'));

        $this->object->addLocale(new Locale('en_US'));
        $this->object->addLocale(new Locale('fr'));
        $this->object->localize('fr');

        $this->assertFalse($this->object->is('en_US'));
        $this->assertTrue($this->object->is('fr'));

        $this->object->localize('en_US');

        $this->assertTrue($this->object->is('en-us')); // Wrong format
        $this->assertFalse($this->object->is('fr'));
    }

    public function testIsEnabled(): void {
        $this->assertFalse($this->object->isEnabled());

        $this->object->addLocale(new Locale('en'));

        $this->assertTrue($this->object->isEnabled());
    }

    public function testLocalizeAndCurrent(): void {
        $en = new Locale('en_US');
        $fr = new Locale('fr');

        $this->object->addLocale($en);
        $this->object->addLocale($fr);

        $this->assertEquals(null, $this->object->current());

        $this->object->localize('fr');

        $this->assertEquals('fr', $this->object->current()->getCode());

        $this->object->localize('en-us'); // Wrong format

        $this->assertEquals('en_US', $this->object->current()->getCode());
    }

    public function testLocalizeSetsConfig(): void {
        $this->assertEquals(null, Config::get('titon.locale.current'));
        $this->assertEquals(null, Config::get('titon.locale.cascade'));

        $this->object->addLocale(new Locale('en_US'));
        $this->object->localize('en_US');

        $this->assertEquals('en_US', Config::get('titon.locale.current'));
        $this->assertEquals(Set {'en_US', 'en'}, Config::get('titon.locale.cascade'));
    }

    public function testSetFallbackSetsConfig(): void {
        $this->assertEquals(null, Config::get('titon.locale.fallback'));

        $this->object->addLocale(new Locale('en'));

        $this->assertEquals('en', Config::get('titon.locale.fallback'));
    }

}
