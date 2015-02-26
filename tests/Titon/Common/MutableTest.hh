<?hh // strict
namespace Titon\Common;

use Titon\Test\Stub\Common\MutableStub;
use Titon\Test\TestCase;

/**
 * @property \Titon\Test\Stub\Common\MutableStub $object
 */
class MutableTest extends TestCase {

    protected function setUp(): void {
        parent::setUp();

        $this->object = new MutableStub();
    }

    public function testAddAll(): void {
        $this->object->set('foo', 'bar');

        $this->assertEquals(Map {'foo' => 'bar'}, $this->object->all());

        $this->object->add(Map {
            'foo' => 'baz',
            'depth.key' => 'value'
        });

        $this->assertEquals(Map {
            'foo' => 'baz',
            'depth' => Map {'key' => 'value'}
        }, $this->object->all());
    }

    public function testFlush(): void {
        $this->object->set('foo', 'bar');

        $this->assertEquals(Map {'foo' => 'bar'}, $this->object->all());

        $this->object->flush();

        $this->assertEquals(Map {}, $this->object->all());
    }

    public function testGet(): void {
        $this->object->set('foo', 'bar');

        $this->assertEquals('bar', $this->object->get('foo'));
        $this->assertEquals('bar', $this->object->foo);

        $this->assertEquals('default', $this->object->get('missing', 'default'));
    }

    public function testHasRemove(): void {
        $this->object->set('foo', 'bar');
        $this->object->set('key', 'value');

        $this->assertTrue($this->object->has('foo'));
        $this->assertTrue(isset($this->object->foo));

        $this->object->remove('foo');
        unset($this->object->key);

        $this->assertFalse($this->object->has('foo'));
        $this->assertFalse(isset($this->object->key));
    }

    public function testSet(): void {
        $this->object->set('a', 1);
        $this->object->b = 2;
        $this->object->c = 3;

        $this->assertEquals(Map {'a' => 1, 'b' => 2, 'c' => 3}, $this->object->all());
        $this->assertEquals(Vector {'a', 'b', 'c'}, $this->object->keys());
        $this->assertEquals(Vector {1, 2, 3}, $this->object->values());
    }

}
