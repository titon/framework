<?hh
namespace Titon\Intl\Utility;

use Titon\Context\Depository;
use Titon\Intl\Locale;
use Titon\Intl\MessageLoader;
use Titon\Intl\Translator;
use Titon\Io\Reader\HackReader;
use Titon\Test\TestCase;

/**
 * @property \Titon\Intl\Translator $object
 */
class NumberTest extends TestCase {

    protected function setUp(): void {
        parent::setUp();

        $this->object = new Translator(new MessageLoader(new HackReader()));
        $this->object->addResourcePaths('common', Set {SRC_DIR . '/Titon/Intl/'});
        $this->object->addLocale(new Locale('en_US'));
        $this->object->localize('en_US');

        Depository::getInstance()->register('Titon\Intl\Translator', $this->object);
    }

    protected function tearDown(): void {
        Depository::getInstance()->clear();
    }

    public function testCurrency(): void {
        $this->assertEquals('$12,345.34', Number::currency(12345.34));
        $this->assertEquals('$734.00', Number::currency(734));
        $this->assertEquals('$84,283.38', Number::currency(84283.384));
        $this->assertEquals('($3,483.23)', Number::currency(-3483.23));
    }

    public function testCurrencyCents(): void {
        $this->assertEquals('0.33&cent;', Number::currency(.33));
        $this->assertEquals('0.75&cent;', Number::currency(0.75));
        $this->assertEquals('(0.75&cent;)', Number::currency(-0.75));
    }

    public function testCurrencyOptions(): void {
        $this->assertEquals('USD 85 839,34', Number::currency(85839.34, Map {
            'use' => 'code',
            'thousands' => ' ',
            'decimals' => ','
        }));

        $this->assertEquals('-$0.34', Number::currency(-0.34, Map {
            'negative' => '-#',
            'cents' => false
        }));
    }

    public function testPercentage(): void {
        $this->assertEquals('123%', Number::percentage(123, Map {'places' => 0}));
        $this->assertEquals('4,546%', Number::percentage(4546, Map {'places' => 0}));
        $this->assertEquals('92,378,453.00%', Number::percentage(92378453));
        $this->assertEquals('3,843.45%', Number::percentage(3843.4450));
        $this->assertEquals('93,789.34%', Number::percentage(93789.34));
    }

    public function testPercentageOptions(): void {
        $this->assertEquals('92 378 453,94%', Number::percentage(92378453.9438, Map {
            'thousands' => ' ',
            'decimals' => ',',
            'places' => 2
        }));
    }
}
