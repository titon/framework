<?hh
namespace Titon\Cache;

use Titon\Cache\Storage\MemoryStorage;
use Titon\Test\TestCase;

/**
 * @property \Titon\Cache\Storage $object
 */
class StorageTest extends TestCase {

    protected function setUp() {
        parent::setUp();

        $this->object = new MemoryStorage();
    }

    public function testExpires() {
        $time = time();

        $this->assertEquals(0, $this->object->expires(0));
        $this->assertEquals(strtotime('+1 hour'), $this->object->expires(null));
        $this->assertEquals(strtotime('+1 day'), $this->object->expires('+1 day'));
        $this->assertEquals($time, $this->object->expires($time));
        $this->assertEquals(3600, $this->object->expires('+60 minutes', true)); // TTL
    }

    public function testKey() {
        $this->assertEquals('Model-someMethod', $this->object->key('Model::someMethod()'));
        $this->assertEquals('Model-someMethod-123456', $this->object->key('Model::someMethod-123456'));
        $this->assertEquals('Model-someMethod--abc-123456', $this->object->key('Model::someMethod()-abc-123456'));
        $this->assertEquals('some-name-space-Model-someMethod', $this->object->key('some\name\space\Model::someMethod()'));
        $this->assertEquals('cache-key', $this->object->key('cache-key'));
    }

    public function testStats() {
        $this->assertInstanceOf('HH\Map', $this->object->stats());
    }

}
