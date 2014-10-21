<?hh
namespace Titon\Common;

use Titon\Test\TestCase;

/**
 * @property \Titon\Common\ReflectableStub $object
 */
class ReflectableTest extends TestCase {

    protected function setUp() {
        parent::setUp();

        $this->object = new ReflectableStub();
    }

    public function testGetReflectionBag() {
        $this->assertInstanceOf('Titon\Common\Bag\ReflectionBag', $this->object->getReflectionBag());
    }

    public function testReflect() {
        $this->assertEquals('Titon\Common\ReflectableStub', $this->object->reflect('className'));
    }

    /**
     * @expectedException \Titon\Common\Exception\InvalidDescriptorException
     */
    public function testReflectInvalidKey() {
        $this->object->reflect('foobar');
    }

}

class ReflectableStub {
    use Reflectable;
}