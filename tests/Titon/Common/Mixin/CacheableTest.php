<?php
namespace Titon\Common\Mixin;

use Titon\Common\Base;
use Titon\Test\TestCase;

/**
 * @property \Titon\Common\Mixin\CacheableStub $object
 */
class CacheableTest extends TestCase {

    protected function setUp() {
        parent::setUp();

        $this->object = new CacheableStub();
        $this->object->setCache('key', 'value');
    }

    public function testCache() {
        $this->object->cache('foo', 'bar');
        $this->assertEquals('bar', $this->object->getCache('foo'));
        $this->object->cache('foo', 'baz'); // doesn't overwrite
        $this->assertEquals('bar', $this->object->getCache('foo'));

        $this->object->cache('number', 12345);
        $this->assertEquals(12345, $this->object->getCache('number'));

        $this->object->cache('closure', function() {
            return (100 * 22);
        });
        $this->assertEquals(2200, $this->object->getCache('closure'));

        $this->object->cache('class', function($parent) {
            return get_class($parent);
        });
        $this->assertEquals('Titon\Common\Mixin\CacheableStub', $this->object->getCache('class'));
    }

    public function testCreateCacheKey() {
        $this->assertEquals('foo', $this->object->createCacheKey('foo'));
        $this->assertEquals('foo-bar', $this->object->createCacheKey(['foo', 'bar']));
        $this->assertEquals('foo-12345-bar', $this->object->createCacheKey(['foo', 12345, 'bar']));
        $this->assertEquals('foo-12345-bar-2282d912cecf739da50a2e91d071b5cc', $this->object->createCacheKey(['foo', 12345, 'bar', ['nested', 'array']]));
    }

    public function testFlushCache() {
        $this->assertEquals(['key' => 'value'], $this->object->allCache());
        $this->object->flushCache();
        $this->assertEquals([], $this->object->allCache());
    }

    public function testGetCache() {
        $this->assertEquals('value', $this->object->getCache('key'));
        $this->assertEquals(null, $this->object->getCache('foo'));

        $this->object->setCache('foo', 'bar');
        $this->assertEquals('bar', $this->object->getCache('foo'));

        $this->object->toggleCache(false);
        $this->assertEquals(null, $this->object->getCache('foo'));

        $this->object->toggleCache(true);
        $this->assertEquals([
            'key' => 'value',
            'foo' => 'bar'
        ], $this->object->allCache());
    }

    public function testHasCache() {
        $this->assertTrue($this->object->hasCache('key'));
        $this->assertFalse($this->object->hasCache('foo'));
    }

    public function testRemoveCache() {
        $this->object->removeCache('key');
        $this->object->removeCache('foo');

        $this->assertEquals([], $this->object->allCache());
    }

    public function testSetCache() {
        $this->assertEquals('bar', $this->object->setCache('foo', 'bar'));
        $this->assertEquals(12345, $this->object->setCache('key', 12345));

        $this->assertEquals([
            'key' => 12345,
            'foo' => 'bar'
        ], $this->object->allCache());
    }

    public function testToggleCache() {
        $this->assertEquals('value', $this->object->getCache('key'));

        $this->object->toggleCache(false);
        $this->assertEquals(null, $this->object->getCache('key'));

        $this->assertEquals('bar', $this->object->cache('foo', 'bar'));
        $this->assertFalse($this->object->hasCache('foo'));

        $this->object->toggleCache(true);
        $this->assertEquals('value', $this->object->getCache('key'));
    }

}

class CacheableStub extends Base {
    use Cacheable;
}