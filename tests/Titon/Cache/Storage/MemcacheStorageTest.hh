<?hh
namespace Titon\Cache\Storage;

use \Memcached;

/**
 * There seems to be many issues with the HHVM/Hack Memcached implementation.
 */
class MemcacheStorageTest extends AbstractStorageTest {

    protected function setUp() {
        if (!extension_loaded('memcached')) {
            $this->markTestSkipped('Memcache is not installed or configured properly');
        }

        $memcache = new Memcached('titon');
        $memcache->addServer('127.0.0.1', '11211');
        $memcache->setOption(Memcached::OPT_BINARY_PROTOCOL, true);
        $memcache->setOption(Memcached::OPT_LIBKETAMA_COMPATIBLE, true);
        $memcache->setOption(Memcached::OPT_COMPRESSION, false);
        $memcache->setOption(Memcached::OPT_DISTRIBUTION, Memcached::DISTRIBUTION_CONSISTENT);

        // Check that memcache connected
        $stats = $memcache->getStats();

        if (empty($stats['127.0.0.1:11211'])) {
            throw new \Exception('Could not connect to Memcache');
        }

        $this->object = new MemcacheStorage($memcache);

        parent::setUp();
    }

}