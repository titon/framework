<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Cache\Storage;

use Titon\Cache\Exception\MissingItemException;
use Titon\Cache\StatsMap;
use \Memcached;

/**
 * A storage engine for the Memcache key-value store; requires pecl/memcached.
 * This engine can be installed using the Cache::addStorage() method.
 *
 * {{{
 *        new MemcacheStorage(new Memcached());
 * }}}
 *
 * @link http://pecl.php.net/package/memcached
 *
 * @package Titon\Cache\Storage
 */
class MemcacheStorage extends AbstractStorage {

    /**
     * The third-party class instance.
     *
     * @var \Memcached
     */
    protected Memcached $memcache;

    /**
     * Set the Memcached instance.
     *
     * @param \Memcached $memcache
     * @param string $prefix
     */
    public function __construct(Memcached $memcache, string $prefix = '') {
        $this->memcache = $memcache;

        parent::__construct($prefix);
    }

    /**
     * {@inheritdoc}
     */
    public function flush(): bool {
        return $this->getMemcache()->flush();
    }

    /**
     * {@inheritdoc}
     */
    public function get(string $key): mixed {
        $value = $this->getMemcache()->get($this->getPrefix() . $key);

        if ($value === false && $this->getMemcache()->getResultCode() === Memcached::RES_NOTFOUND) {
            throw new MissingItemException(sprintf('Item with key %s does not exist', $key));
        }

        return $value;
    }

    /**
     * Return the Memcached instance.
     *
     * @return \Memcached
     */
    public function getMemcache(): Memcached {
        return $this->memcache;
    }

    /**
     * {@inheritdoc}
     */
    public function has(string $key): bool {
        return (
            $this->getMemcache()->get($this->getPrefix() . $key) &&
            $this->getMemcache()->getResultCode() === Memcached::RES_SUCCESS
        );
    }

    /**
     * {@inheritdoc}
     */
    public function remove(string $key): bool {
        return $this->getMemcache()->delete($this->getPrefix() . $key);
    }

    /**
     * {@inheritdoc}
     */
    public function set(string $key, mixed $value, int $expires): bool {
        return $this->getMemcache()->set($this->getPrefix() . $key, $value, $expires);
    }

    /**
     * {@inheritdoc}
     */
    public function stats(): StatsMap {
        $servers = $this->getMemcache()->getServerList();
        $stats = $this->getMemcache()->getStats();
        $stats = $stats[$servers[0]['host'] . ':' . $servers[0]['port']];

        return Map {
            self::HITS => $stats['get_hits'],
            self::MISSES => $stats['get_misses'],
            self::UPTIME => $stats['uptime'],
            self::MEMORY_USAGE => $stats['bytes'],
            self::MEMORY_AVAILABLE => $stats['limit_maxbytes']
        };
    }

}
