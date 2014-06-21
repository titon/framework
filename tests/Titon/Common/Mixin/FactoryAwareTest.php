<?php
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Common\Mixin;

use Titon\Common\Base;
use Titon\Common\Registry;
use Titon\Test\TestCase;

/**
 * @property \Titon\Common\Mixin\FactoryStub $object
 */
class FactoryAwareTest extends TestCase {

    protected function tearDown() {
        parent::tearDown();

        Registry::flush();
    }

    public function testFactory() {
        $instance1 = FactoryStub::factory();
        $this->assertInstanceOf('Titon\Common\Mixin\FactoryStub', $instance1);

        $instance2 = FactoryStub::factory();
        $this->assertInstanceOf('Titon\Common\Mixin\FactoryStub', $instance2);

        $this->assertNotSame($instance1, $instance2);
    }

    public function testRegistry() {
        $instance = FactoryStub::registry();

        $this->assertInstanceOf('Titon\Common\Mixin\FactoryStub', $instance);
        $this->assertSame($instance, FactoryStub::registry());
    }

}

class FactoryStub extends Base {
    use FactoryAware;
}