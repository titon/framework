<?hh
namespace Titon\Cache\Storage;

use \Redis;

class RedisStorageTest extends AbstractStorageTest {

    protected function setUp() {
        if (!extension_loaded('redis')) {
            $this->markTestSkipped('Redis is not installed or configured properly');
        }

        $redis = new Redis();
        $redis->pconnect('127.0.0.1', '6379');

        // Check that redis connected
        if ($redis->ping() !== '+PONG') {
            $this->markTestSkipped('Could not connect to Redis');
        }

        $this->object = new RedisStorage($redis, 'redis-');

        parent::setUp();
    }

}
