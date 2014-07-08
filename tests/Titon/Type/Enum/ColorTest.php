<?hh
namespace Titon\Type\Enum;

use Titon\Test\TestCase;

/**
 * @property \Titon\Type\Enum\Color $object
 */
class ColorTest extends TestCase {

    protected function setUp() {
        parent::setUp();

        $this->object = new Color(Color::GRAY);
    }

    public function testVariables() {
        $this->assertEquals('808080', $this->object->hex);
        $this->assertEquals(128, $this->object->r);
        $this->assertEquals(128, $this->object->g);
        $this->assertEquals(128, $this->object->b);
    }

    public function testIs() {
        $this->assertTrue($this->object->is(Color::GRAY));
        $this->assertFalse($this->object->is(Color::RED));
    }

}