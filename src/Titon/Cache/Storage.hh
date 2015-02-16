<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Cache;

/**
 * Interface for the storage containers library.
 *
 * @package Titon\Cache
 */
interface Storage {

    const string HITS = 'hits';
    const string MISSES = 'misses';
    const string UPTIME = 'uptime';
    const string MEMORY_USAGE = 'memory.usage';
    const string MEMORY_AVAILABLE = 'memory.available';

    /**
     * Deletes all items in the pool.
     *
     * @return boolean
     *   True if the pool was successfully cleared. False if there was an error.
     */
    public function clear(): bool;

    /**
     * Persists any deferred cache items.
     *
     * @return bool
     *   TRUE if all not-yet-saved items were successfully saved. FALSE otherwise.
     */
    public function commit(): bool;

    /**
     * Decrement a value within the cache and return the new number.
     * If the item does not exist, it will create the item with an initial value.
     *
     * @param string $key
     * @param int $step
     * @param int $initial
     * @return int
     */
    public function decrement(string $key, int $step = 1, int $initial = 0): int;

    /**
     * Delete a single item from the pool.
     *
     * @param string $key
     * @return $this
     */
    public function deleteItem(string $key): this;

    /**
     * Removes multiple items from the pool.
     *
     * @param array $keys
     *   An array of keys that should be removed from the pool.
     * @return $this
     */
    public function deleteItems(array<string> $keys): this;

    /**
     * Empty the cache. Alias for clear().
     *
     * @return bool
     */
    public function flush(): bool;

    /**
     * Return the raw value from the storage pool instead of returning an item.
     * If the item does not exist, throw a MissingItemException.
     *
     * @param string $key
     * @return mixed
     * @throws \Titon\Cache\Exception\MissingItemException
     */
    public function get(string $key): mixed;

    /**
     * Returns a Cache Item representing the specified key.
     *
     * This method must always return an ItemInterface object, even in case of
     * a cache miss. It MUST NOT return null.
     *
     * @param string $key
     *   The key for which to return the corresponding Cache Item.
     * @return \Titon\Cache\Item
     */
    public function getItem(string $key): Item;

    /**
     * Returns a traversable set of cache items.
     *
     * @param array $keys
     * An indexed array of keys of items to retrieve.
     * @return array|\Traversable
     * A traversable collection of Cache Items keyed by the cache keys of
     * each item. A Cache item will be returned for each key, even if that
     * key is not found. However, if no keys are specified then an empty
     * traversable MUST be returned instead.
     */
    public function getItems(array<string> $keys = []): ItemMap;

    /**
     * Return the unique cache key prefix.
     *
     * @return string
     */
    public function getPrefix(): string;

    /**
     * Check if the item exists within the cache.
     *
     * @param string $key
     * @return bool
     */
    public function has(string $key): bool;

    /**
     * Increment a value within the cache and return the new number.
     * If the item does not exist, it will create the item with an initial value.
     *
     * @param string $key
     * @param int $step
     * @param int $initial
     * @return int
     */
    public function increment(string $key, int $step = 1, int $initial = 0): int;

    /**
     * Remove the item if it exists and return true, else return false.
     *
     * @param string $key
     * @return bool
     */
    public function remove(string $key): bool;

    /**
     * Persists a cache item immediately.
     *
     * @param \Titon\Cache\Item $item
     *   The cache item to save.
     *
     * @return $this
     */
    public function save(Item $item): this;

    /**
     * Sets a cache item to be persisted later.
     *
     * @param \Titon\Cache\Item $item
     *   The cache item to save.
     * @return $this
     */
    public function saveDeferred(Item $item): this;

    /**
     * Write data to the storage cache directly.
     *
     * @param string $key
     * @param mixed $value
     * @param int $expires
     * @return bool
     */
    public function set(string $key, mixed $value, int $expires): bool;

    /**
     * Set the unique cache key prefix.
     *
     * @param string $prefix
     * @return $this
     */
    public function setPrefix(string $prefix): this;

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
    public function store(string $key, CacheCallback $callback, mixed $expires = null): mixed;

}
