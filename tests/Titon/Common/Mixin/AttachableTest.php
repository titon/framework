<?php
namespace Titon\Common\Mixin;

use Titon\Common\Bag\ConfigBag;
use Titon\Common\Base;
use Titon\Common\Registry;
use Titon\Test\TestCase;

/**
 * @property \Titon\Common\Mixin\AttachableStub $object
 */
class AttachableTest extends TestCase {

    protected function setUp() {
        parent::setUp();

        $this->object = new AttachableStub();

        // by closure
        $this->object->attachObject('base', function() {
            return new Base();
        });

        // by class
        $this->object->attachObject([
            'alias' => 'config',
            'class' => 'Titon\Common\Config'
        ]);

        // by property
        $this->object->registry = function() {
            return new Registry();
        };
    }

    public function testAttachObject() {
        $this->assertInstanceOf('Titon\Common\Base', $this->object->base);
        $this->assertInstanceOf('Titon\Common\Config', $this->object->config);
        $this->assertInstanceOf('Titon\Common\Registry', $this->object->registry);
    }

    public function testAttachObjectWithInterface() {
        $this->object->attachObject([
            'alias' => 'bag',
            'class' => 'Titon\Common\Bag\ConfigBag',
            'interface' => 'ArrayAccess'
        ]);

        $this->assertInstanceOf('Titon\Common\Bag\ConfigBag', $this->object->bag);
    }

    /**
     * @expectedException \Titon\Common\Exception\UnsupportedInterfaceException
     */
    public function testAttachObjectWithInterfaceFails() {
        $this->object->attachObject([
            'alias' => 'bag',
            'interface' => 'Iterator',
        ], function() {
            return new ConfigBag([]);
        });

        $this->assertInstanceOf('Titon\Common\Bag\ConfigBag', $this->object->bag);
    }

    public function testAttachObjectNoRegistry() {
        $this->object->attachObject([
            'alias' => 'bag',
            'class' => 'Titon\Common\Bag\ConfigBag',
            'register' => false
        ]);

        $this->assertInstanceOf('Titon\Common\Bag\ConfigBag', $this->object->bag);
    }

    /**
     * @expectedException \Titon\Common\Exception\InvalidObjectException
     */
    public function testAttachObjectErrorsNoAlias() {
        $this->object->attachObject(['class' => 'Titon\Common\Base']);
    }

    /**
     * @expectedException \Titon\Common\Exception\InvalidObjectException
     */
    public function testAttachObjectErrorsNoClass() {
        $this->object->attachObject(['alias' => 'something']);
    }

    public function testDetachObject() {
        $this->assertTrue(isset($this->object->relation));
        $this->object->detachObject('relation');
        $this->assertFalse(isset($this->object->relation));

        $this->assertTrue(isset($this->object->config));
        unset($this->object->config);
        $this->assertFalse(isset($this->object->config));
    }

    public function testGetObject() {
        $this->assertInstanceOf('Titon\Common\Base', $this->object->base);
        $this->assertInstanceOf('Titon\Common\Base', $this->object->getObject('base'));

        $this->assertInstanceOf('Titon\Common\Config', $this->object->config);
        $this->assertInstanceOf('Titon\Common\Config', $this->object->getObject('config'));

        $this->assertInstanceOf('Titon\Common\Registry', $this->object->registry);
        $this->assertInstanceOf('Titon\Common\Registry', $this->object->getObject('registry'));
    }

    /**
     * @expectedException \Titon\Common\Exception\MissingObjectException
     */
    public function testGetObjectMissingKey() {
        $this->object->getObject('fake');
    }

    public function testHasObject() {
        $this->object->base->setConfig('key', 'Forcing this object to be instantiated!');

        $this->assertTrue(isset($this->object->base));
        $this->assertTrue($this->object->hasObject('base'));

        $this->assertFalse($this->object->hasObject('fake'));
    }

    public function testNotifyObjects() {
        $this->object->attachObject('mock1', new NotifyStub());
        $this->object->attachObject('mock2', new NotifyStub());
        $this->object->attachObject('mock3', new NotifyStub());

        // Call method
        $this->object->notifyObjects('doNotify');
        $this->assertEquals(1, $this->object->mock1->count);
        $this->assertEquals(1, $this->object->mock2->count);
        $this->assertEquals(1, $this->object->mock3->count);

        // Restrict and call with argument
        $this->object->restrictObject('mock3');
        $this->object->notifyObjects('doNotify', [3]);
        $this->assertEquals(4, $this->object->mock1->count);
        $this->assertEquals(4, $this->object->mock2->count);
        $this->assertEquals(1, $this->object->mock3->count);

        // Allow
        $this->object->allowObject('mock3');
        $this->object->notifyObjects('doNotify', [1]);
        $this->assertEquals(5, $this->object->mock1->count);
        $this->assertEquals(5, $this->object->mock2->count);
        $this->assertEquals(2, $this->object->mock3->count);
    }

    public function testChaining() {
        $this->assertInstanceOf('Titon\Common\Mixin\AttachableStub', $this->object->relation);

        // We can go as deep as we want
        $this->assertInstanceOf('Titon\Common\Mixin\AttachableStub', $this->object->relation->relation->relation->relation->relation->relation->relation->relation->relation->relation->relation);
    }
}

class AttachableStub extends Base {
    use Attachable;

    public function initialize() {
        $this->attachObject('relation', function() {
            return new AttachableStub();
        });
    }
}

class NotifyStub {

    public $count = 0;

    public function doNotify($step = 1) {
        $this->count += $step;
    }
}