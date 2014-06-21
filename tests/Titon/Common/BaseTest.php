<?php
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
        $this->assertInstanceOf('Titon\Common\Bag\ConfigBag', $this->object->getConfigBag());
        $this->assertInstanceOf('Titon\Common\Bag\ReflectionBag', $this->object->getReflectionBag());
    }

    public function testSerialize() {
        $this->assertEquals('a:1:{s:10:"initialize";b:1;}', $this->object->serialize());
        $this->assertEquals(['initialize' => true], $this->object->jsonSerialize());
    }

    public function testToString() {
        $this->assertEquals('Titon\Common\Base', $this->object->toString());
        $this->assertEquals('Titon\Common\Base', (string) $this->object);
    }

    public function testUnserialize() {
        $this->object->unserialize('a:1:{s:10:"initialize";b:1;}');

        $this->assertEquals(['initialize' => true], $this->object->allConfig());
    }

    public function testNoop() {
        $this->assertEquals(null, $this->object->noop());
    }

}