<?hh
namespace Titon\Common;

use Titon\Common\Base;
use Titon\Test\TestCase;

/**
 * @property \Titon\Common\ConfigurableStub $object
 */
class ConfigurableTest extends TestCase {

    protected function setUp() {
        parent::setUp();

        $this->object = new ConfigurableStub();
    }

    public function testApplyAndAllConfig() {
        $this->assertEquals(Map {'initialize' => true, 'foo' => 'bar', 'cfg' => true}, $this->object->allConfig());
    }

    public function testAddAllConfig() {
        $this->object->setConfig('foo', 'bar');
        $this->object->setConfig('key', 'value');

        $this->assertEquals(Map {
            'initialize' => true,
            'foo' => 'bar',
            'cfg' => true,
            'key' => 'value'
        }, $this->object->allConfig());

        $this->object->addConfig(Map {
            'foo' => 'baz',
            'cfg' => false
        });

        $this->assertEquals(Map {
            'initialize' => true,
            'foo' => 'baz',
            'cfg' => false,
            'key' => 'value'
        }, $this->object->allConfig());
    }

    public function testGetSetConfig() {
        $this->assertEquals('bar', $this->object->getConfig('foo'));
        $this->object->setConfig('foo', 'baz');
        $this->assertEquals('baz', $this->object->getConfig('foo'));

        $this->assertEquals(null, $this->object->getConfig('key'));
        $this->object->setConfig('key.key', 'value');
        $this->assertEquals(Map {'key' => 'value'}, $this->object->getConfig('key'));
    }

    public function testHasRemoveConfig() {
        $this->object->setConfig('foo', 'bar');
        $this->assertTrue($this->object->hasConfig('foo'));

        $this->object->removeConfig('foo');
        $this->assertFalse($this->object->hasConfig('foo'));
    }

}

class ConfigurableStub extends Base {
    use Configurable;

    protected Map<string, mixed> $_config = Map {
        'foo' => 'bar',
        'cfg' => true
    };

}