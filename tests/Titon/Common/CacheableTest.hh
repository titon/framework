<?hh
namespace Titon\Common;

use Titon\Test\TestCase;

/**
 * @property \Titon\Common\CacheableStub $object
 */
class CacheableTest extends TestCase {

    protected function setUp() {
        parent::setUp();

        $this->object = new CacheableStub();
        $this->object->setCache('key', 'value');
    }

    public function testCache() {
        $this->object->cache('foo', function() {
            return 'bar';
        });
        $this->assertEquals('bar', $this->object->getCache('foo'));

        // doesn't overwrite
        $this->object->cache('foo', function() {
            return 'baz';
        });
        $this->assertEquals('bar', $this->object->getCache('foo'));

        $this->object->cache('number', function() {
            return 12345;
        });
        $this->assertEquals(12345, $this->object->getCache('number'));

        $this->object->cache('closure', function() {
            return (100 * 22);
        });
        $this->assertEquals(2200, $this->object->getCache('closure'));

        $this->object->cache('class', function($parent) {
            return get_class($parent);
        });
        $this->assertEquals('Titon\Common\CacheableStub', $this->object->getCache('class'));

        $this->object->cache('false', function() {
            return false;
        });
        $this->assertEquals(false, $this->object->getCache('false'));

        $this->object->cache('null', function() {
            return null;
        });
        $this->assertEquals(null, $this->object->getCache('null'));

        $this->object->cache('zero', function() {
            return 0;
        });
        $this->assertEquals(0, $this->object->getCache('zero'));

        $this->object->cache('array', function() {
            return [];
        });
        $this->assertEquals([], $this->object->getCache('array'));
    }

    public function testCreateCacheKey() {
        $this->assertEquals('foo', $this->object->createCacheKey('foo'));
        $this->assertEquals('foo-bar', $this->object->createCacheKey(['foo', 'bar']));
        $this->assertEquals('foo-12345-bar', $this->object->createCacheKey(['foo', 12345, 'bar']));
        $this->assertEquals('foo-12345-bar-d3e545e5b6dd7d1c9c7be76d5bb18241', $this->object->createCacheKey(['foo', 12345, 'bar', ['nested', 'array']]));
    }

    public function testFlushCache() {
        $this->assertEquals(Map {'key' => 'value'}, $this->object->allCache());
        $this->object->flushCache();
        $this->assertEquals(Map {}, $this->object->allCache());
    }

    public function testGetCache() {
        $this->assertEquals('value', $this->object->getCache('key'));
        $this->assertEquals(null, $this->object->getCache('foo'));

        $this->object->setCache('foo', 'bar');
        $this->assertEquals('bar', $this->object->getCache('foo'));

        $this->object->toggleCache(false);
        $this->assertEquals(null, $this->object->getCache('foo'));

        $this->object->toggleCache(true);
        $this->assertEquals(Map {
            'key' => 'value',
            'foo' => 'bar'
        }, $this->object->allCache());
    }

    public function testHasCache() {
        $this->assertTrue($this->object->hasCache('key'));
        $this->assertFalse($this->object->hasCache('foo'));
    }

    public function testRemoveCache() {
        $this->object->removeCache('key');
        $this->object->removeCache('foo');

        $this->assertEquals(Map {}, $this->object->allCache());
    }

    public function testSetCache() {
        $this->assertEquals('bar', $this->object->setCache('foo', 'bar'));
        $this->assertEquals(12345, $this->object->setCache('key', 12345));

        $this->assertEquals(Map {
            'key' => 12345,
            'foo' => 'bar'
        }, $this->object->allCache());
    }

    public function testToggleCache() {
        $this->assertEquals('value', $this->object->getCache('key'));

        $this->object->toggleCache(false);
        $this->assertEquals(null, $this->object->getCache('key'));

        $this->assertEquals('bar', $this->object->setCache('foo', 'bar'));
        $this->assertFalse($this->object->hasCache('foo'));

        $this->object->toggleCache(true);
        $this->assertEquals('value', $this->object->getCache('key'));
    }

}

class CacheableStub {
    use Cacheable;
}