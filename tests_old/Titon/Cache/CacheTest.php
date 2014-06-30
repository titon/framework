<?php
namespace Titon\Cache;

use Titon\Cache\Storage\MemoryStorage;
use Titon\Test\TestCase;

/**
 * @property \Titon\Cache\Cache $object
 */
class CacheTest extends TestCase {

    protected function setUp() {
        parent::setUp();

        $this->object = new Cache();

        $default = new MemoryStorage();
        $default->set('key', 'foo');

        $custom = new MemoryStorage();
        $custom->set('key', 'bar');

        $this->object->addStorage('default', $default);
        $this->object->addStorage('custom', $custom);
    }

    public function testDecrement() {
        $this->assertEquals(null, $this->object->get('decrement'));
        $this->assertEquals(null, $this->object->get('decrement', 'custom'));

        $this->object->set('decrement', 0);
        $this->object->decrement('decrement', 1);

        $this->object->set('decrement', 0, '+1 day', 'custom');
        $this->object->decrement('decrement', 5, 'custom');

        $this->assertEquals(-1, $this->object->get('decrement'));
        $this->assertEquals(-5, $this->object->get('decrement', 'custom'));
    }

    public function testFlush() {
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

    public function testGet() {
        $this->assertEquals(null, $this->object->get('fakeKey'));
        $this->assertEquals('foo', $this->object->get('key'));

        $this->assertEquals(null, $this->object->get('fakeKey', 'custom'));
        $this->assertEquals('bar', $this->object->get('key', 'custom'));
    }

    public function testHas() {
        $this->assertTrue($this->object->has('key'));
        $this->assertFalse($this->object->has('fakeKey'));

        $this->assertTrue($this->object->has('key', 'custom'));
        $this->assertFalse($this->object->has('fakeKey', 'custom'));
    }

    public function testIncrement() {
        $this->assertEquals(null, $this->object->get('increment'));
        $this->assertEquals(null, $this->object->get('increment', 'custom'));

        $this->object->set('increment', 0);
        $this->object->increment('increment', 4);

        $this->object->set('increment', 0, '+1 day', 'custom');
        $this->object->increment('increment', 2, 'custom');

        $this->assertEquals(4, $this->object->get('increment'));
        $this->assertEquals(2, $this->object->get('increment', 'custom'));
    }

    public function testRemove() {
        $this->assertEquals('foo', $this->object->get('key'));
        $this->object->remove('key');
        $this->assertEquals(null, $this->object->get('key'));

        $this->assertEquals('bar', $this->object->get('key', 'custom'));
        $this->object->remove('key', 'custom');
        $this->assertEquals(null, $this->object->get('key', 'custom'));
    }

    public function testSet() {
        $this->assertEquals('foo', $this->object->get('key'));
        $this->object->set('key', 'bar', '+1 hour');
        $this->assertEquals('bar', $this->object->get('key'));

        $this->assertEquals('bar', $this->object->get('key', 'custom'));
        $this->object->set('key', 'baz', null, 'custom');
        $this->assertEquals('baz', $this->object->get('key', 'custom'));
    }

    public function testStats() {
        $this->assertTrue(is_array($this->object->stats()));
    }

    public function testStorages() {
        $this->object->addStorage('test', new MemoryStorage(['storage' => 'test']));

        $this->assertInstanceOf('Titon\Cache\Storage', $this->object->getStorage('test'));
        $this->assertEquals('test', $this->object->getStorage('test')->getConfig('storage'));
        $this->assertEquals(['default', 'custom', 'test'], array_keys($this->object->getStorages()));
    }

    /**
     * @expectedException \Titon\Cache\Exception\MissingStorageException
     */
    public function testGetStorageMissing() {
        $this->object->getStorage('missing');
    }

    public function testStore() {
        $this->assertEquals('bar', $this->object->store('foo', function() {
            return 'bar';
        }));

        $this->assertEquals('bar', $this->object->store('foo', function() {
            return 'baz';
        }));
    }

}