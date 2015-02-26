<?hh // strict
namespace Titon\Cache;

use Titon\Cache\Storage\MemoryStorage;
use Titon\Test\TestCase;

/**
 * @property \Titon\Cache\Cache $object
 */
class CacheTest extends TestCase {

    protected function setUp(): void {
        parent::setUp();

        $this->object = new Cache();

        $default = new MemoryStorage();
        $default->set('key', 'foo', 300);

        $custom = new MemoryStorage();
        $custom->set('key', 'bar', 300);

        $this->object->addStorage('default', $default);
        $this->object->addStorage('custom', $custom);
    }

    public function testDecrement(): void {
        $this->assertEquals(null, $this->object->get('decrement')->get());
        $this->assertEquals(null, $this->object->get('decrement', 'custom')->get());

        $this->object->set('decrement', 0);
        $this->object->decrement('decrement', 1);

        $this->object->set('decrement', 0, '+1 day', 'custom');
        $this->object->decrement('decrement', 5, 0, 'custom');

        $this->assertEquals(-1, $this->object->get('decrement')->get());
        $this->assertEquals(-5, $this->object->get('decrement', 'custom')->get());
    }

    public function testFlush(): void {
        $this->object->set('test', 123);
        $this->assertTrue($this->object->has('key'));
        $this->assertTrue($this->object->has('test'));

        $this->object->set('test', 123, null, 'custom');
        $this->assertTrue($this->object->has('key', 'custom'));
        $this->assertTrue($this->object->has('test', 'custom'));

        // flush only by name
        $this->object->flush('default');
        $this->assertFalse($this->object->has('key'));
        $this->assertFalse($this->object->has('test'));
        $this->assertTrue($this->object->has('key', 'custom'));
        $this->assertTrue($this->object->has('test', 'custom'));

        // flush all
        $this->object->flush();
        $this->assertFalse($this->object->has('key'));
        $this->assertFalse($this->object->has('test'));
        $this->assertFalse($this->object->has('key', 'custom'));
        $this->assertFalse($this->object->has('test', 'custom'));
    }

    public function testGet(): void {
        $this->assertEquals(null, $this->object->get('fakeKey')->get());
        $this->assertEquals('foo', $this->object->get('key')->get());

        $this->assertEquals(null, $this->object->get('fakeKey', 'custom')->get());
        $this->assertEquals('bar', $this->object->get('key', 'custom')->get());
    }

    public function testHas(): void {
        $this->assertTrue($this->object->has('key'));
        $this->assertFalse($this->object->has('fakeKey'));

        $this->assertTrue($this->object->has('key', 'custom'));
        $this->assertFalse($this->object->has('fakeKey', 'custom'));
    }

    public function testIncrement(): void {
        $this->assertEquals(null, $this->object->get('increment')->get());
        $this->assertEquals(null, $this->object->get('increment', 'custom')->get());

        $this->object->set('increment', 0);
        $this->object->increment('increment', 4);

        $this->object->set('increment', 0, '+1 day', 'custom');
        $this->object->increment('increment', 2, 0, 'custom');

        $this->assertEquals(4, $this->object->get('increment')->get());
        $this->assertEquals(2, $this->object->get('increment', 'custom')->get());
    }

    public function testRemove(): void {
        $this->assertEquals('foo', $this->object->get('key')->get());

        $this->object->remove('key');

        $this->assertEquals(null, $this->object->get('key')->get());

        $this->assertEquals('bar', $this->object->get('key', 'custom')->get());

        $this->object->remove('key', 'custom');

        $this->assertEquals(null, $this->object->get('key', 'custom')->get());
    }

    public function testSet(): void {
        $this->assertEquals('foo', $this->object->get('key')->get());

        $this->object->set('key', 'bar', '+1 hour');

        $this->assertEquals('bar', $this->object->get('key')->get());

        $this->assertEquals('bar', $this->object->get('key', 'custom')->get());

        $this->object->set('key', 'baz', null, 'custom');

        $this->assertEquals('baz', $this->object->get('key', 'custom')->get());
    }

    public function testStats(): void {
        $this->assertInstanceOf('HH\Map', $this->object->stats());
    }

    public function testStorages(): void {
        $this->object->addStorage('test', new MemoryStorage());

        $this->assertInstanceOf('Titon\Cache\Storage', $this->object->getStorage('test'));
        $this->assertEquals(Vector {'default', 'custom', 'test'}, $this->object->getStorages()->keys());
    }

    /**
     * @expectedException \Titon\Cache\Exception\MissingStorageException
     */
    public function testGetStorageMissing(): void {
        $this->object->getStorage('missing');
    }

    public function testStore(): void {
        $this->assertEquals('bar', $this->object->store('foo', () ==> 'bar'));

        $this->assertEquals('bar', $this->object->store('foo', () ==> 'baz'));
    }

}
