<?hh
namespace Titon\Common;

use Titon\Test\TestCase;

class InstanceableTest extends TestCase {

    public function testMultiton() {
        $object = InstanceableStub::getInstance();
        $object->key = 1;

        $this->assertInstanceOf('Titon\Common\InstanceableStub', $object);
        $this->assertEquals(1, InstanceableStub::countInstances());

        $object2 = InstanceableStub::getInstance('alternate');
        $object2->key = 2;

        $this->assertInstanceOf('Titon\Common\InstanceableStub', $object);
        $this->assertEquals(2, InstanceableStub::countInstances());

        // Test differences
        $this->assertEquals(1, InstanceableStub::getInstance()->key);
        $this->assertEquals(2, InstanceableStub::getInstance('alternate')->key);

        // Remove
        InstanceableStub::removeInstance('alternate');
        $this->assertEquals(1, InstanceableStub::countInstances());

        // Flush
        InstanceableStub::flushInstances();
        $this->assertEquals(0, InstanceableStub::countInstances());
    }

    public function testSingleton() {
        $object = InstanceableStub::getInstance();
        $object->key = 1;

        $this->assertInstanceOf('Titon\Common\InstanceableStub', $object);
        $this->assertEquals(1, InstanceableStub::countInstances());

        $object2 = InstanceableStub::getInstance();

        $this->assertInstanceOf('Titon\Common\InstanceableStub', $object);
        $this->assertEquals(1, InstanceableStub::countInstances());
        $this->assertEquals(1, InstanceableStub::getInstance()->key);

        $object2->key = 2;

        // Test differences
        $this->assertEquals(2, $object->key);
        $this->assertEquals(2, InstanceableStub::getInstance()->key);

        // Flush
        InstanceableStub::flushInstances();
        $this->assertEquals(0, InstanceableStub::countInstances());
    }

}

class InstanceableStub {
    use Instanceable<InstanceableStub>;
}