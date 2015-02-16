<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Cache\Storage;

use Titon\Cache\Exception\MissingItemException;
use Titon\Cache\StatsMap;
use \Redis;

/**
 * A storage engine for the Redis key-value store; requires the redis modules.
 * This engine can be installed using the Cache::addStorage() method.
 *
 * {{{
 *        new RedisStorage(new Redis());
 * }}}
 *
 * @link https://github.com/nicolasff/phpredis
 *
 * @package Titon\Cache\Storage
 */
class RedisStorage extends AbstractStorage {

    /**
     * The third-party class instance.
     *
     * @var \Redis
     */
    protected Redis $redis;

    /**
     * Set the Redis instance.
     *
     * @param \Redis $redis
     * @param string $prefix
     */
    public function __construct(Redis $redis, string $prefix = '') {
        $this->redis = $redis;

        parent::__construct($prefix);
    }

    /**
     * {@inheritdoc}
     */
    public function flush(): bool {
        return $this->getRedis()->flushDB();
    }

    /**
     * {@inheritdoc}
     */
    public function get(string $key): mixed {
        $value = $this->getRedis()->get($this->getPrefix() . $key);

        if ($value === false) {
            throw new MissingItemException(sprintf('Item with key %s does not exist', $key));
        }

        return unserialize($value);
    }

    /**
     * Return the Redis instance.
     *
     * @return \Redis
     */
    public function getRedis(): Redis {
        return $this->redis;
    }

    /**
     * {@inheritdoc}
     */
    public function has(string $key): bool {
        return $this->getRedis()->exists($this->getPrefix() . $key);
    }

    /**
     * {@inheritdoc}
     */
    public function remove(string $key): bool {
        return (bool) $this->getRedis()->delete($this->getPrefix() . $key);
    }

    /**
     * {@inheritdoc}
     */
    public function set(string $key, mixed $value, int $expires): bool {
        return $this->getRedis()->setex($this->getPrefix() . $key, $expires - time(), serialize($value)); // Redis is TTL
    }

    /**
     * {@inheritdoc}
     */
    public function stats(): StatsMap {
        $stats = $this->getRedis()->info();

        return Map {
            self::HITS => $stats['keyspace_hits'],
            self::MISSES => $stats['keyspace_misses'],
            self::UPTIME => $stats['uptime_in_seconds'],
            self::MEMORY_USAGE => $stats['used_memory'],
            self::MEMORY_AVAILABLE => false
        };
    }

}
