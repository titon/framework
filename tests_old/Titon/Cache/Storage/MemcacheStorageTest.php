<?hh
namespace Titon\Cache\Storage;

/**
 * There seems to be many issues with the HHVM/Hack Memcached implementation.
 */
class MemcacheStorageTest extends AbstractStorageTest {

    protected function setUp() {
        if (!extension_loaded('memcached')) {
            $this->markTestSkipped('Memcache is not installed or configured properly');
        }

        $this->object = new MemcacheStorage(Map {
            'server' => '127.0.0.1:11211'
        });

        // Check that memcache connected
        $stats = $this->object->connection->getStats();

        if (empty($stats['127.0.0.1:11211'])) {
            throw new \Exception('Could not connect to Memcache');
        }

        parent::setUp();
    }

}