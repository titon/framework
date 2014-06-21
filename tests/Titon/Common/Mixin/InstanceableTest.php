<?php
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Common\Mixin;

use Titon\Common\Base;
use Titon\Test\TestCase;

class InstanceableTest extends TestCase {

    public function testMultiton() {
        $object = InstanceableStub::getInstance();
        $object->setConfig('key', 1);

        $this->assertInstanceOf('Titon\Common\Mixin\InstanceableStub', $object);
        $this->assertEquals(1, InstanceableStub::countInstances());

        $object2 = InstanceableStub::getInstance('alternate');
        $object2->setConfig('key', 2);

        $this->assertInstanceOf('Titon\Common\Mixin\InstanceableStub', $object);
        $this->assertEquals(2, InstanceableStub::countInstances());

        // Test differences
        $this->assertEquals(1, InstanceableStub::getInstance()->getConfig('key'));
        $this->assertEquals(2, InstanceableStub::getInstance('alternate')->getConfig('key'));

        // Remove
        InstanceableStub::removeInstance('alternate');
        $this->assertEquals(1, InstanceableStub::countInstances());

        // Flush
        InstanceableStub::flushInstances();
        $this->assertEquals(0, InstanceableStub::countInstances());
    }

    public function testSingleton() {
        $object = InstanceableStub::getInstance();
        $object->setConfig('key', 1);

        $this->assertInstanceOf('Titon\Common\Mixin\InstanceableStub', $object);
        $this->assertEquals(1, InstanceableStub::countInstances());

        $object2 = InstanceableStub::getInstance();

        $this->assertInstanceOf('Titon\Common\Mixin\InstanceableStub', $object);
        $this->assertEquals(1, InstanceableStub::countInstances());
        $this->assertEquals(1, InstanceableStub::getInstance()->getConfig('key'));

        $object2->setConfig('key', 2);

        // Test differences
        $this->assertEquals(2, $object->getConfig('key'));
        $this->assertEquals(2, InstanceableStub::getInstance()->getConfig('key'));

        // Flush
        InstanceableStub::flushInstances();
        $this->assertEquals(0, InstanceableStub::countInstances());
    }

}

class InstanceableStub extends Base {
    use Instanceable;
}