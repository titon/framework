<?php
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Cache;

use \Closure;

/**
 * Interface for the storage containers library.
 *
 * @package Titon\Cache
 */
interface Storage {

    const HITS = 'hits';
    const MISSES = 'misses';
    const UPTIME = 'uptime';
    const MEMORY_USAGE = 'memoryUsage';
    const MEMORY_AVAILABLE = 'memoryAvailable';

    /**
     * Decrement a value within the cache and return the new number.
     * Return false if the item cannot be found.
     *
     * @param string $key
     * @param int $step
     * @return int|bool
     */
    public function decrement($key, $step = 1);

    /**
     * Empty the cache.
     *
     * @return bool
     */
    public function flush();

    /**
     * Get data from the cache if it exists.
     *
     * @param string $key
     * @return mixed Returns null if the key isn't found, else the value
     */
    public function get($key);

    /**
     * Check if the item exists within the cache.
     *
     * @param string $key
     * @return bool
     */
    public function has($key);

    /**
     * Increment a value within the cache and return the new number.
     * Return false if the item cannot be found.
     *
     * @param string $key
     * @param int $step
     * @return int|bool
     */
    public function increment($key, $step = 1);

    /**
     * Remove the item if it exists and return true, else return false.
     *
     * @param string $key
     * @return bool
     */
    public function remove($key);

    /**
     * Set data to the cache.
     *
     * @param string $key
     * @param mixed $value
     * @param mixed $expires
     * @return bool
     */
    public function set($key, $value, $expires = '+1 day');

    /**
     * Returns cached information from the storage engine.
     *
     * @return array
     */
    public function stats();

    /**
     * Read and write cache using a callback.
     *
     * @param string $key
     * @param \Closure $callback
     * @param mixed $expires
     * @return mixed
     */
    public function store($key, Closure $callback, $expires = '+1 day');

}