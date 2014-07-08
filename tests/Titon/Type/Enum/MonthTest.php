<?hh
namespace Titon\Type\Enum;

use Titon\Test\TestCase;

/**
 * @property \Titon\Type\Enum\Day $object
 */
class MonthTest extends TestCase {

    protected function setUp() {
        parent::setUp();

        $this->object = new Month(Month::FEBRUARY);
    }

    public function testVariables() {
        $this->assertEquals(2, $this->object->order);
        $this->assertEquals('February', $this->object->name);
        $this->assertEquals('Feb', $this->object->shortName);
        $this->assertEquals('february', $this->object->slug);
        $this->assertTrue(in_array($this->object->daysInMonth, [28, 29]));
    }

    public function testIs() {
        $this->assertTrue($this->object->is(Month::FEBRUARY));
        $this->assertFalse($this->object->is(Month::DECEMBER));
    }

}