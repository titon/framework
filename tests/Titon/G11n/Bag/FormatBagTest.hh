<?hh
namespace Titon\G11n\Bag;

use Titon\G11n\Locale;
use Titon\Test\TestCase;

/**
 * @property \Titon\G11n\Bag\FormatBag $object
 */
class FormatBagTest extends TestCase {

    protected function setUp(): void {
        parent::setUp();

        $locale = new Locale('en');
        $locale->addResourcePath('core', SRC_DIR . '/Titon/G11n/');

        $this->object = $locale->getFormatPatterns();
    }

    public function testGetCurrency(): void {
        $this->assertEquals(Map {
            'code' => 'USD #',
            'dollar' => '$#',
            'cents' => '#&cent;',
            'negative' => '(#)',
            'use' => 'dollar'
        }, $this->object->getCurrency());
    }

    public function testGetDate(): void {
        $this->assertEquals('%m/%d/%Y', $this->object->getDate());
    }

    public function testGetDatetime(): void {
        $this->assertEquals('%m/%d/%Y %I:%M%p', $this->object->getDatetime());
    }

    public function testGetNumber(): void {
        $this->assertEquals(Map {
            'thousands' => ',',
            'decimals' => '.',
            'places' => '2'
        }, $this->object->getNumber());
    }

    public function testGetPhone(): void {
        $this->assertEquals(Map {
            7 => '###-####',
            10 => '(###) ###-####',
            11 => '# (###) ###-####'
        }, $this->object->getPhone());
    }

    public function testGetSSN(): void {
        $this->assertEquals('###-##-####', $this->object->getSSN());
    }

    public function testGetTime(): void {
        $this->assertEquals('%I:%M%p', $this->object->getTime());
    }

}
