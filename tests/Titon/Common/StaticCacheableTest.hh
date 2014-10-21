<?hh
namespace Titon\Common;

use Titon\Test\TestCase;

class StaticCacheableTest extends TestCase {

    protected function setUp() {
        parent::setUp();

        StaticCacheableStub::flushCache();
        StaticCacheableStub::setCache('key', 'value');
    }

    public function testCache() {
        StaticCacheableStub::cache('foo', 'bar');
        $this->assertEquals('bar', StaticCacheableStub::getCache('foo'));
        StaticCacheableStub::cache('foo', 'baz'); // doesn't overwrite
        $this->assertEquals('bar', StaticCacheableStub::getCache('foo'));

        StaticCacheableStub::cache('number', 12345);
        $this->assertEquals(12345, StaticCacheableStub::getCache('number'));

        StaticCacheableStub::cache('closure', function() {
            return (100 * 22);
        });
        $this->assertEquals(2200, StaticCacheableStub::getCache('closure'));

        StaticCacheableStub::cache('class', function() {
            return 'Titon\Common\StaticCacheableStub';
        });
        $this->assertEquals('Titon\Common\StaticCacheableStub', StaticCacheableStub::getCache('class'));
    }

    public function testCreateCacheKey() {
        $this->assertEquals('foo', StaticCacheableStub::createCacheKey('foo'));
        $this->assertEquals('foo-bar', StaticCacheableStub::createCacheKey(['foo', 'bar']));
        $this->assertEquals('foo-12345-bar', StaticCacheableStub::createCacheKey(['foo', 12345, 'bar']));
        $this->assertEquals('foo-12345-bar-2282d912cecf739da50a2e91d071b5cc', StaticCacheableStub::createCacheKey(['foo', 12345, 'bar', ['nested', 'array']]));
    }

    public function testFlushCache() {
        $this->assertEquals(Map {'key' => 'value'}, StaticCacheableStub::allCache());
        StaticCacheableStub::flushCache();
        $this->assertEquals(Map {}, StaticCacheableStub::allCache());
    }

    public function testGetCache() {
        $this->assertEquals('value', StaticCacheableStub::getCache('key'));
        $this->assertEquals(null, StaticCacheableStub::getCache('foo'));

        StaticCacheableStub::setCache('foo', 'bar');
        $this->assertEquals('bar', StaticCacheableStub::getCache('foo'));

        $this->assertEquals(Map {
            'key' => 'value',
            'foo' => 'bar'
        }, StaticCacheableStub::allCache());
    }

    public function testHasCache() {
        $this->assertTrue(StaticCacheableStub::hasCache('key'));
        $this->assertFalse(StaticCacheableStub::hasCache('foo'));
    }

    public function testRemoveCache() {
        StaticCacheableStub::removeCache('key');
        StaticCacheableStub::removeCache('foo');

        $this->assertEquals(Map {}, StaticCacheableStub::allCache());
    }

    public function testSetCache() {
        $this->assertEquals('bar', StaticCacheableStub::setCache('foo', 'bar'));
        $this->assertEquals(12345, StaticCacheableStub::setCache('key', 12345));

        $this->assertEquals(Map {
            'key' => 12345,
            'foo' => 'bar'
        }, StaticCacheableStub::allCache());
    }

}

class StaticCacheableStub {
    use StaticCacheable;
}