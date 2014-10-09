<?hh
namespace Titon\Common;

use Titon\Test\TestCase;

/**
 * @property \Titon\Common\Base $object
 */
class BaseTest extends TestCase {

    protected function setUp() {
        parent::setUp();

        $this->object = new Base();
    }

    public function testBags() {
        $this->assertInstanceOf('Titon\Common\Bag\ReflectionBag', $this->object->getReflectionBag());
    }

    public function testToString() {
        $this->assertEquals('Titon\Common\Base', $this->object->toString());
        $this->assertEquals('Titon\Common\Base', (string) $this->object);
    }

    public function testNoop() {
        $this->assertEquals(null, $this->object->noop());
    }

}