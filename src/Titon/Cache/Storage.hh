<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Cache;

type CacheCallback = (function(): mixed);
type StatsMap = Map<string, mixed>;

/**
 * Interface for the storage containers library.
 *
 * @package Titon\Cache
 */
interface Storage {

    const string HITS = 'hits';
    const string MISSES = 'misses';
    const string UPTIME = 'uptime';
    const string MEMORY_USAGE = 'memoryUsage';
    const string MEMORY_AVAILABLE = 'memoryAvailable';

    /**
     * Decrement a value within the cache and return the new number.
     * Return null if the item cannot be found.
     *
     * @param string $key
     * @param int $step
     * @return int
     */
    public function decrement(string $key, int $step = 1): ?int;

    /**
     * Empty the cache.
     *
     * @return bool
     */
    public function flush(): bool;

    /**
     * Get data from the cache if it exists.
     *
     * @param string $key
     * @return mixed Returns null if the key isn't found, else the value
     */
    public function get(string $key): mixed;

    /**
     * Check if the item exists within the cache.
     *
     * @param string $key
     * @return bool
     */
    public function has(string $key): bool;

    /**
     * Increment a value within the cache and return the new number.
     * Return null if the item cannot be found.
     *
     * @param string $key
     * @param int $step
     * @return int
     */
    public function increment(string $key, int $step = 1): ?int;

    /**
     * Remove the item if it exists and return true, else return false.
     *
     * @param string $key
     * @return bool
     */
    public function remove(string $key): bool;

    /**
     * Set data to the cache.
     *
     * @param string $key
     * @param mixed $value
     * @param mixed $expires
     * @return bool
     */
    public function set(string $key, mixed $value, mixed $expires = '+1 day'): bool;

    /**
     * Returns cached information from the storage engine.
     *
     * @return \Titon\Cache\StatsMap
     */
    public function stats(): StatsMap;

    /**
     * Read and write cache using a callback.
     * Return the value of the read cache or the value being set.
     *
     * @param string $key
     * @param \Titon\Cache\CacheCallback $callback
     * @param mixed $expires
     * @return mixed
     */
    public function store(string $key, CacheCallback $callback, mixed $expires = '+1 day'): mixed;

}