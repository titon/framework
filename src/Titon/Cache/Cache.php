<?hh //strict
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
     * @type \Titon\Cache\Storage[]
     */
    protected $_storage = [];

    /**
     * Add a new storage engine to the cache system.
     *
     * @param string $key
     * @param \Titon\Cache\Storage $storage
     * @return \Titon\Cache\Storage
     */
    public function addStorage($key, Storage $storage) {
        $this->_storage[$key] = $storage;

        return $storage;
    }

    /**
     * Decrement a value within the cache.
     *
     * @param string $key
     * @param int $step
     * @param string $storage
     * @return bool
     */
    public function decrement($key, $step = 1, $storage = 'default') {
        return $this->getStorage($storage)->decrement($key, $step);
    }

    /**
     * Empty the cache.
     *
     * @param string $storage
     * @return bool
     */
    public function flush($storage = null) {
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
    public function get($key, $storage = 'default') {
        return $this->getStorage($storage)->get($key);
    }

    /**
     * Retrieve the storage engine if it exists.
     *
     * @param string $key
     * @return \Titon\Cache\Storage
     * @throws \Titon\Cache\Exception\MissingStorageException
     */
    public function getStorage($key) {
        if (isset($this->_storage[$key])) {
            return $this->_storage[$key];
        }

        throw new MissingStorageException(sprintf('Cache storage engine %s does not exist', $key));
    }

    /**
     * Return all storage engines.
     *
     * @return \Titon\Cache\Storage[]
     */
    public function getStorages() {
        return $this->_storage;
    }

    /**
     * Check to see if the cached item is within storage.
     *
     * @param string $key
     * @param string $storage
     * @return bool
     */
    public function has($key, $storage = 'default') {
        return $this->getStorage($storage)->has($key);
    }

    /**
     * Increment a value within the cache.
     *
     * @param string $key
     * @param int $step
     * @param string $storage
     * @return bool
     */
    public function increment($key, $step = 1, $storage = 'default') {
        return $this->getStorage($storage)->increment($key, $step);
    }

    /**
     * Remove the item if it exists and return true, else return false.
     *
     * @param string $key
     * @param string $storage
     * @return bool
     */
    public function remove($key, $storage = 'default') {
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
    public function set($key, $value, $expires = '+1 day', $storage = 'default') {
        return $this->getStorage($storage)->set($key, $value, $expires);
    }

    /**
     * Returns cached information from the storage engine.
     *
     * @param string $storage
     * @return array
     */
    public function stats($storage = 'default') {
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
    public function store($key, Closure $callback, $expires = '+1 day', $storage = 'default') {
        return $this->getStorage($storage)->store($key, $callback, $expires);
    }

}