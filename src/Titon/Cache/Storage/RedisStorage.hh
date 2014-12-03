<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Cache\Storage;

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
     * @type \Redis
     */
    protected Redis $_redis;

    /**
     * Set the Redis instance.
     *
     * @param \Redis $redis
     */
    public function __construct(Redis $redis) {
        $this->_redis = $redis;
    }

    /**
     * {@inheritdoc}
     */
    public function decrement(string $key, int $step = 1): ?int {
        return $this->getRedis()->decrBy($this->key($key), $step);
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
        $value = $this->getRedis()->get($this->key($key));

        if ($value === false) {
            return null;
        }

        return $value;
    }

    /**
     * Return the Redis instance.
     *
     * @return \Redis
     */
    public function getRedis(): Redis {
        return $this->_redis;
    }

    /**
     * {@inheritdoc}
     */
    public function has(string $key): bool {
        return $this->getRedis()->exists($this->key($key));
    }

    /**
     * {@inheritdoc}
     */
    public function increment(string $key, int $step = 1): ?int {
        return $this->getRedis()->incrBy($this->key($key), $step);
    }

    /**
     * {@inheritdoc}
     */
    public function remove(string $key): bool {
        $this->getRedis()->delete($this->key($key));

        return true;
    }

    /**
     * {@inheritdoc}
     */
    public function set(string $key, mixed $value, mixed $expires = '+1 day'): bool {
        if ($expires === 0) {
            return $this->getRedis()->set($this->key($key), $value);
        }

        $expires = $this->expires($expires, true);

        // Immediately invalidate
        if ($expires < 0) {
            return true;
        }

        return $this->getRedis()->setex($this->key($key), $expires, $value);
    }

    /**
     * {@inheritdoc}
     */
    public function stats(): StatsMap {
        $stats = $this->getRedis()->info();

        return Map {
            self::HITS => false,
            self::MISSES => false,
            self::UPTIME => $stats['uptime_in_seconds'],
            self::MEMORY_USAGE => $stats['used_memory'],
            self::MEMORY_AVAILABLE => false
        };
    }

}