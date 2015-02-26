<?hh // strict
namespace Titon\Common;

use Titon\Test\Stub\Common\StaticCacheableStub;
use Titon\Test\TestCase;

class StaticCacheableTest extends TestCase {

    protected function setUp(): void {
        parent::setUp();

        StaticCacheableStub::flushCache();
        StaticCacheableStub::setCache('key', 'value');
    }

    public function testCache(): void {
        StaticCacheableStub::cache('foo', function(): void {
            return 'bar';
        });
        $this->assertEquals('bar', StaticCacheableStub::getCache('foo'));

        // doesn't overwrite
        StaticCacheableStub::cache('foo', function(): void {
            return 'baz';
        });
        $this->assertEquals('bar', StaticCacheableStub::getCache('foo'));

        StaticCacheableStub::cache('number', function(): void {
            return 12345;
        });
        $this->assertEquals(12345, StaticCacheableStub::getCache('number'));

        StaticCacheableStub::cache('closure', function(): void {
            return (100 * 22);
        });
        $this->assertEquals(2200, StaticCacheableStub::getCache('closure'));

        StaticCacheableStub::cache('class', function(): void {
            return 'Titon\Common\StaticCacheableStub';
        });
        $this->assertEquals('Titon\Common\StaticCacheableStub', StaticCacheableStub::getCache('class'));

        StaticCacheableStub::cache('false', function(): void {
            return false;
        });
        $this->assertEquals(false, StaticCacheableStub::getCache('false'));

        StaticCacheableStub::cache('null', function(): void {
            return null;
        });
        $this->assertEquals(null, StaticCacheableStub::getCache('null'));

        StaticCacheableStub::cache('zero', function(): void {
            return 0;
        });
        $this->assertEquals(0, StaticCacheableStub::getCache('zero'));

        StaticCacheableStub::cache('array', function(): void {
            return [];
        });
        $this->assertEquals([], StaticCacheableStub::getCache('array'));
    }

    public function testCreateCacheKey(): void {
        $this->assertEquals('foo', StaticCacheableStub::createCacheKey('foo'));
        $this->assertEquals('foo-bar', StaticCacheableStub::createCacheKey(['foo', 'bar']));
        $this->assertEquals('foo-12345-bar', StaticCacheableStub::createCacheKey(['foo', 12345, 'bar']));
        $this->assertEquals('foo-12345-bar-d3e545e5b6dd7d1c9c7be76d5bb18241', StaticCacheableStub::createCacheKey(['foo', 12345, 'bar', ['nested', 'array']]));
    }

    public function testFlushCache(): void {
        $this->assertEquals(Map {'key' => 'value'}, StaticCacheableStub::allCache());
        StaticCacheableStub::flushCache();
        $this->assertEquals(Map {}, StaticCacheableStub::allCache());
    }

    public function testGetCache(): void {
        $this->assertEquals('value', StaticCacheableStub::getCache('key'));
        $this->assertEquals(null, StaticCacheableStub::getCache('foo'));

        StaticCacheableStub::setCache('foo', 'bar');
        $this->assertEquals('bar', StaticCacheableStub::getCache('foo'));

        $this->assertEquals(Map {
            'key' => 'value',
            'foo' => 'bar'
        }, StaticCacheableStub::allCache());
    }

    public function testHasCache(): void {
        $this->assertTrue(StaticCacheableStub::hasCache('key'));
        $this->assertFalse(StaticCacheableStub::hasCache('foo'));
    }

    public function testRemoveCache(): void {
        StaticCacheableStub::removeCache('key');
        StaticCacheableStub::removeCache('foo');

        $this->assertEquals(Map {}, StaticCacheableStub::allCache());
    }

    public function testSetCache(): void {
        $this->assertEquals('bar', StaticCacheableStub::setCache('foo', 'bar'));
        $this->assertEquals(12345, StaticCacheableStub::setCache('key', 12345));

        $this->assertEquals(Map {
            'key' => 12345,
            'foo' => 'bar'
        }, StaticCacheableStub::allCache());
    }

}
