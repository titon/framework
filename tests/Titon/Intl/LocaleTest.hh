<?hh
namespace Titon\Intl;

use Titon\Intl\Bag\FormatBag;
use Titon\Intl\Bag\InflectionBag;
use Titon\Intl\Bag\MetadataBag;
use Titon\Intl\Bag\ValidationBag;
use Titon\Test\TestCase;
use Titon\Utility\Config;

/**
 * @property \Titon\Intl\Locale $object
 */
class LocaleTest extends TestCase {

    protected function setUp(): void {
        parent::setUp();

        $this->object = new Locale('ex_CH');
        $this->object->addResourcePath('common', TEMP_DIR . '/intl/');
    }

    public function testCodeIsCanonicalized(): void {
        $this->assertEquals('ex_CH', $this->object->getCode());
    }

    public function testBundlesAreAutoInitialized(): void {
        $this->assertInstanceOf('Titon\Intl\LocaleBundle', $this->object->getLocaleBundle());
        $this->assertInstanceOf('Titon\Intl\MessageBundle', $this->object->getMessageBundle());
    }

    public function testParentIsAutoInitialized(): void {
        $this->assertInstanceOf('Titon\Intl\Locale', $this->object->getParentLocale());
        $this->assertEquals('ex', $this->object->getParentLocale()->getCode());
    }

    public function testAddResourcePath(): void {
        $this->object->addResourcePath('foo', __DIR__);

        $this->assertEquals(Map {
            'common' => Set { TEMP_DIR . '/intl/locales/ex_CH/', __DIR__ . '/locales/ex_CH/' }
        }, $this->object->getLocaleBundle()->getPaths());

        $this->assertEquals(Map {
            'common' => Set { TEMP_DIR . '/intl/messages/ex_CH/' },
            'foo' => Set { __DIR__ . '/messages/ex_CH/' }
        }, $this->object->getMessageBundle()->getPaths());
    }

    public function testAddResourcePaths(): void {
        $this->object->addResourcePaths('bar', Set {
            __DIR__,
            dirname(__DIR__)
        });

        $this->assertEquals(Map {
            'common' => Set {
                TEMP_DIR . '/intl/locales/ex_CH/',
                __DIR__ . '/locales/ex_CH/',
                dirname(__DIR__) . '/locales/ex_CH/'
            }
        }, $this->object->getLocaleBundle()->getPaths());

        $this->assertEquals(Map {
            'common' => Set { TEMP_DIR . '/intl/messages/ex_CH/' },
            'bar' => Set { __DIR__ . '/messages/ex_CH/', dirname(__DIR__) . '/messages/ex_CH/' }
        }, $this->object->getMessageBundle()->getPaths());
    }

    public function testCanonicalize(): void {
        $this->assertEquals('en-us', Locale::canonicalize('en-us', Locale::FORMAT_1));
        $this->assertEquals('en-US', Locale::canonicalize('en-us', Locale::FORMAT_2));
        $this->assertEquals('en_US', Locale::canonicalize('en-us', Locale::FORMAT_3));
        $this->assertEquals('enUS', Locale::canonicalize('en-us', Locale::FORMAT_4));

        $this->assertEquals('en-us', Locale::canonicalize('en-US', Locale::FORMAT_1));
        $this->assertEquals('en-US', Locale::canonicalize('en-US', Locale::FORMAT_2));
        $this->assertEquals('en_US', Locale::canonicalize('en-US', Locale::FORMAT_3));
        $this->assertEquals('enUS', Locale::canonicalize('en-US', Locale::FORMAT_4));

        $this->assertEquals('en-us', Locale::canonicalize('en_US', Locale::FORMAT_1));
        $this->assertEquals('en-US', Locale::canonicalize('en_US', Locale::FORMAT_2));
        $this->assertEquals('en_US', Locale::canonicalize('en_US', Locale::FORMAT_3));
        $this->assertEquals('enUS', Locale::canonicalize('en_US', Locale::FORMAT_4));
    }

    public function testCompose(): void {
        $this->assertEquals('en', Locale::compose(Map {
            'language' => 'en'
        }));

        $this->assertEquals('en_US', Locale::compose(Map {
            'language' => 'en',
            'region' => 'US'
        }));

        $this->assertEquals('en_Hans_US', Locale::compose(Map {
            'language' => 'en',
            'region' => 'US',
            'script' => 'Hans'
        }));

        $this->assertEquals('en_Hans_US_NEDIS_x_prv1', Locale::compose(Map {
            'language' => 'en',
            'region' => 'US',
            'script' => 'Hans',
            'variant0' => 'NEDIS',
            'private0' => 'prv1'
        }));
    }

    public function testDecompose(): void {
        $this->assertEquals(Map {
            'language' => 'en'
        }, Locale::decompose('en'));

        $this->assertEquals(Map {
            'language' => 'en',
            'region' => 'US'
        }, Locale::decompose('en_US'));

        $this->assertEquals(Map {
            'language' => 'en',
            'region' => 'US',
            'script' => 'Hans'
        }, Locale::decompose('en_Hans_US'));

        $this->assertEquals(Map {
            'language' => 'en',
            'script' => 'Hans',
            'region' => 'US',
            'variant0' => 'NEDIS',
            'private0' => 'prv1'
        }, Locale::decompose('en_Hans_US_nedis_x_prv1'));
    }

    public function testGetCode(): void {
        $this->assertEquals('ex_CH', $this->object->getCode());
        $this->assertEquals('ex', $this->object->getParentLocale()->getCode());
    }

    public function testGetFormatPatterns(): void {
        $this->assertEquals(new FormatBag(Map {
            'date' => '%Y/%m/%d',
            'time' => '%I:%M%p',
            'datetime' => '%m/%d/%Y %I:%M%p',
            'ssn' => '###-##-####'
        }), $this->object->getFormatPatterns());

        $this->assertEquals(new FormatBag(Map {
            'date' => '%m/%d/%Y',
            'time' => '%I:%M%p',
            'datetime' => '%m/%d/%Y %I:%M%p',
        }), $this->object->getParentLocale()->getFormatPatterns());
    }

    public function testGetInflectionRules(): void {
        $this->assertEquals(new InflectionBag(Map {
            'irregular' => Map {
                'atlas' => 'atlases',
                'hoof' => 'hoofs',
            },
            'uninflected' => Set {'equipment', 'nankingese'}
        }), $this->object->getInflectionRules());

        $this->assertEquals(new InflectionBag(Map {
            'irregular' => Map {
                'atlas' => 'atlases'
            },
            'uninflected' => Set {'equipment'}
        }), $this->object->getParentLocale()->getInflectionRules());
    }

    public function testGetMetadata(): void {
        $this->assertEquals(new MetadataBag(Map {
            'code' => 'ex_CH',
            'iso2' => 'ex',
            'iso3' => 'exa',
            'timezone' => 'America/New_York',
            'title' => 'Example Child',
            'parent' => 'ex',
            'plural' => PluralRule::RULE_1
        }), $this->object->getMetadata());

        $this->assertEquals(new MetadataBag(Map {
            'code' => 'ex',
            'iso2' => 'ex',
            'iso3' => 'exa',
            'timezone' => 'America/New_York',
            'title' => 'Example',
            'plural' => PluralRule::RULE_1
        }), $this->object->getParentLocale()->getMetadata());
    }

    public function testGetParentLocale(): void {
        $this->assertInstanceOf('Titon\Intl\Locale', $this->object->getParentLocale());
        $this->assertEquals(null, $this->object->getParentLocale()->getParentLocale());
    }

    public function testGetValidationRules(): void {
        $this->assertEquals(new ValidationBag(Map {
            'phone' => '/^([0-9]{7}$/'
        }), $this->object->getValidationRules());

        // No child file, so parent should match
        $this->assertEquals(new ValidationBag(Map {
            'phone' => '/^([0-9]{7}$/'
        }), $this->object->getParentLocale()->getValidationRules());
    }

}
