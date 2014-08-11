<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Cache;

use Titon\Cache\Exception\MissingStorageException;
use Titon\Cache\Storage;
use Titon\Common\FactoryAware;
use \Closure;

/**
 * Provides a very basic interface for caching individual sets of data. Multiple storage engines can be setup
 * to support different caching mechanisms: Memcache, APC, XCache, Memory, FileSystem, etc.
 *
 * @package Titon\Cache
 */
class Cache {
    use FactoryAware;

    /**
     * Storage engines.
     *
     * @type Map<string, Storage>
     */
    protected Map<string, Storage> $_storage = Map {};

    /**
     * Add a new storage engine to the cache system.
     *
     * @param string $key
     * @param \Titon\Cache\Storage $storage
     * @return $this
     */
    public function addStorage(string $key, Storage $storage): this {
        $this->_storage[$key] = $storage;

        return $this;
    }

    /**
     * Decrement a value within the cache.
     *
     * @param string $key
     * @param int $step
     * @param string $storage
     * @return int
     */
    public function decrement(string $key, int $step = 1, string $storage = 'default'): ?int {
        return $this->getStorage($storage)->decrement($key, $step);
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
     * @return mixed
     */
    public function get(string $key, string $storage = 'default'): ?mixed {
        return $this->getStorage($storage)->get($key);
    }

    /**
     * Retrieve the storage engine if it exists.
     *
     * @param string $key
     * @return \Titon\Cache\Storage
     * @throws \Titon\Cache\Exception\MissingStorageException
     */
    public function getStorage(string $key): Storage {
        if (isset($this->_storage[$key])) {
            return $this->_storage[$key];
        }

        throw new MissingStorageException(sprintf('Cache storage engine %s does not exist', $key));
    }

    /**
     * Return all storage engines.
     *
     * @return Map<string, Storage>
     */
    public function getStorages(): Map<string, Storage> {
        return $this->_storage;
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
     * @param string $storage
     * @return int
     */
    public function increment(string $key, int $step = 1, string $storage = 'default'): ?int {
        return $this->getStorage($storage)->increment($key, $step);
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
    public function set(string $key, ?mixed $value, mixed $expires = '+1 day', string $storage = 'default'): bool {
        return $this->getStorage($storage)->set($key, $value, $expires);
    }

    /**
     * Returns cached information from the storage engine.
     *
     * @param string $storage
     * @return Map<string, mixed>
     */
    public function stats(string $storage = 'default'): Map<string, mixed> {
        return $this->getStorage($storage)->stats();
    }

    /**
     * Get or set data to the storage engine.
     *
     * @param string $key
     * @param \Closure $callback
     * @param mixed $expires
     * @param string $storage
     * @return mixed
     */
    public function store(string $key, Closure $callback, mixed $expires = '+1 day', string $storage = 'default'): ?mixed {
        return $this->getStorage($storage)->store($key, $callback, $expires);
    }

}