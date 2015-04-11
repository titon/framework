<?hh
namespace Titon\View\Locator;

use Titon\Test\TestCase;
use Titon\Utility\Config;
use Titon\View\Template;
use Titon\View\ViewTest;

/**
 * @property \Titon\View\Locator\TemplateLocator $object
 */
class LocaleTemplateLocatorTest extends TestCase {

    protected function setUp(): void {
        parent::setUp();

        $this->vfs()->createStructure(ViewTest::generateViewStructure());

        $this->object = new LocaleTemplateLocator([
            $this->vfs()->path('/views'),
            $this->vfs()->path('/views-fallback')
        ], ['en', 'fr']);
    }

    public function testAddGetSetLocales(): void {
        $this->assertEquals(Vector {'en', 'fr'}, $this->object->getLocales());

        $this->object->addLocale('en');
        $this->object->addLocale('de');

        $this->assertEquals(Vector {'en', 'fr', 'de'}, $this->object->getLocales());

        $this->object->addLocales(Vector {'en-us', 'fr', 'it', 'nl'});

        $this->assertEquals(Vector {'en', 'fr', 'de', 'en-us', 'it', 'nl'}, $this->object->getLocales());

        $this->object->setLocales(Vector {'en', 'en', 'es'});

        $this->assertEquals(Vector {'en', 'es'}, $this->object->getLocales());
    }

    public function testConfigLocalesAreLoaded(): void {
        Config::set('titon.locale.cascade', Vector {'en-us', 'en-gb', 'en', 'es-co', 'es'});

        $object = new LocaleTemplateLocator('/views', ['it']);

        $this->assertEquals(Vector {
            'it',
            'en-us',
            'en-gb',
            'en',
            'es-co',
            'es'
        }, $object->getLocales());
    }

    public function testBuildTemplateLookup(): void {
        $this->assertEquals(Vector {
            'foo/bar.en.tpl',
            'foo/bar.fr.tpl',
            'foo/bar.tpl'
        }, $this->object->buildTemplateLookup('foo/bar'));
    }

    public function testLocate(): void {
        $this->assertEquals($this->vfs()->path('/views/public/lang/index.fr.tpl'), $this->object->locate('lang/index'));
    }

    public function testLocateFallThrough(): void {
        $this->object->setLocales(Vector {'en'});

        $this->assertEquals($this->vfs()->path('/views/public/lang/index.tpl'), $this->object->locate('lang/index'));
    }

    public function testLocateDoubleLocaleCode(): void {
        $this->object->setLocales(Vector {'en-us'});

        $this->assertEquals($this->vfs()->path('/views/public/lang/index.en-us.tpl'), $this->object->locate('lang/index'));
    }

}

