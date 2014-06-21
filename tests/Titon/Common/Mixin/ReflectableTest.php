<?php
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Common\Mixin;

use Titon\Common\Base;
use Titon\Test\TestCase;

/**
 * @property \Titon\Common\Base $object
 */
class ReflectableTest extends TestCase {

    protected function setUp() {
        parent::setUp();

        $this->object = new Base();
    }

    public function testGetReflectionBag() {
        $this->assertInstanceOf('Titon\Common\Bag\ReflectionBag', $this->object->getReflectionBag());
    }

    public function testReflect() {
        $this->assertEquals('Titon\Common\Base', $this->object->reflect('className'));
    }

    /**
     * @expectedException \Titon\Common\Exception\InvalidDescriptorException
     */
    public function testReflectInvalidKey() {
        $this->object->reflect('foobar');
    }

}