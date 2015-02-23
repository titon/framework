<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Cache;

use Titon\Cache\Exception\MissingStorageException;
use Titon\Cache\Storage;

/**
 * Provides a very basic interface for caching individual sets of data. Multiple storage engines can be setup
 * to support different caching mechanisms: Memcache, APC, XCache, Memory, FileSystem, etc.
 *
 * @package Titon\Cache
 */
class Cache {

    /**
     * Storage engines.
     *
     * @var \Titon\Cache\StorageMap
     */
    protected StorageMap $storage = Map {};

    /**
     * Add a new storage engine to the cache system.
     *
     * @param string $key
     * @param \Titon\Cache\Storage $storage
     * @return $this
     */
    public function addStorage(string $key, Storage $storage): this {
        $this->storage[$key] = $storage;

        return $this;
    }

    /**
     * Decrement a value within the cache.
     *
     * @param string $key
     * @param int $step
     * @param int $initial
     * @param string $storage
     * @return int
     */
    public function decrement(string $key, int $step = 1, int $initial = 0, string $storage = 'default'): int {
        return $this->getStorage($storage)->decrement($key, $step, $initial);
    }

    /**
     * Empty the cache.
     *
     * @param string $storage
     * @return bool
     */
    public function flush(string $storage = ''): bool {
        if ($storage) {
            return $this->getStorage($storage)->flush();
        } else {
            foreach ($this->getStorages() as $storage) {
                $storage->flush();
            }
        }

        return true;
    }

    /**
     * Get data from the storage engine defined by the key.
     *
     * @param string $key
     * @param string $storage
     * @return \Titon\Cache\Item
     */
    public function get(string $key, string $storage = 'default'): Item {
        return $this->getStorage($storage)->getItem($key);
    }

    /**
     * Retrieve the storage engine if it exists.
     *
     * @param string $key
     * @return \Titon\Cache\Storage
     * @throws \Titon\Cache\Exception\MissingStorageException
     */
    public function getStorage(string $key): Storage {
        if ($this->storage->contains($key)) {
            return $this->storage[$key];
        }

        throw new MissingStorageException(sprintf('Cache storage engine %s does not exist', $key));
    }

    /**
     * Return all storage engines.
     *
     * @return \Titon\Cache\StorageMap
     */
    public function getStorages(): StorageMap {
        return $this->storage;
    }

    /**
     * Check to see if the cached item is within storage.
     *
     * @param string $key
     * @param string $storage
     * @return bool
     */
    public function has(string $key, string $storage = 'default'): bool {
        return $this->getStorage($storage)->has($key);
    }

    /**
     * Increment a value within the cache.
     *
     * @param string $key
     * @param int $step
     * @param int $initial
     * @param string $storage
     * @return int
     */
    public function increment(string $key, int $step = 1, int $initial = 0, string $storage = 'default'): int {
        return $this->getStorage($storage)->increment($key, $step, $initial);
    }

    /**
     * Remove the item if it exists and return true, else return false.
     *
     * @param string $key
     * @param string $storage
     * @return bool
     */
    public function remove(string $key, string $storage = 'default'): bool {
        return $this->getStorage($storage)->remove($key);
    }

    /**
     * Set data to the defined storage engine.
     *
     * @param string $key
     * @param mixed $value
     * @param mixed $expires
     * @param string $storage
     * @return bool
     */
    public function set(string $key, mixed $value, mixed $expires = null, string $storage = 'default'): bool {
        $this->getStorage($storage)->save(new Item($key, $value, $expires));

        return true;
    }

    /**
     * Returns cached information from the storage engine.
     *
     * @param string $storage
     * @return \Titon\Cache\StatsMap
     */
    public function stats(string $storage = 'default'): StatsMap {
        return $this->getStorage($storage)->stats();
    }

    /**
     * Get or set data to the storage engine.
     *
     * @param string $key
     * @param \Titon\Cache\CacheCallback $callback
     * @param mixed $expires
     * @param string $storage
     * @return mixed
     */
    public function store(string $key, CacheCallback $callback, mixed $expires = null, string $storage = 'default'): mixed {
        return $this->getStorage($storage)->store($key, $callback, $expires);
    }

}
