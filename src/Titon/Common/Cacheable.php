<?hh
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Common;

use \Closure;

/**
 * The Cacheable trait provides functionality to cache any data from the class layer.
 * All data is unique and represented by a generated cache key.
 *
 * @package Titon\Common
 */
trait Cacheable {

    /**
     * Cached items indexed by key.
     *
     * @type array
     */
    protected $_cache = [];

    /**
     * Is cache on or off?
     *
     * @type bool
     */
    private $__cacheEnabled = true;

    /**
     * Return all the current cached items.
     *
     * @return array
     */
    public function allCache() {
        return $this->_cache;
    }

    /**
     * Dynamically read and write from the cache at once. If the cache exists with the key return it, else execute and save the result.
     * If the value happens to be a closure, evaluate the closure and save the result.
     *
     * @param array|string $key
     * @param mixed|\Closure $value
     * @return mixed
     */
    public function cache($key, $value) {
        $key = $this->createCacheKey($key);

        if ($cache = $this->getCache($key)) {
            return $cache;
        }

        if ($value instanceof Closure) {
            $value = call_user_func($value, $this);
        }

        if (!$this->__cacheEnabled) {
            return $value;
        }

        return $this->setCache($key, $value);
    }

    /**
     * Generate a cache key. If an array is passed, drill down and form a key.
     *
     * @param string|array $keys
     * @return string
     */
    public function createCacheKey($keys) {
        if (is_array($keys)) {
            $key = array_shift($keys);

            if ($keys) {
                foreach ($keys as $value) {
                    if (is_array($value)) {
                        $key .= '-' . md5(json_encode($value));
                    } else if ($value) {
                        $key .= '-' . $value;
                    }
                }
            }
        } else {
            $key = $keys;
        }

        return (string) $key;
    }

    /**
     * Empty the cache.
     *
     * @return $this
     */
    public function flushCache() {
        $this->_cache = [];

        return $this;
    }

    /**
     * Return a cached item if it exists, else return null.
     *
     * @param string|array $key
     * @return mixed
     */
    public function getCache($key) {
        if (!$this->__cacheEnabled) {
            return null;
        }

        $key = $this->createCacheKey($key);

        if ($this->hasCache($key)) {
            return $this->_cache[$key];
        }

        return null;
    }

    /**
     * Check to see if the cache key exists.
     *
     * @param string|array $key
     * @return bool
     */
    public function hasCache($key) {
        return isset($this->_cache[$this->createCacheKey($key)]);
    }

    /**
     * Remove an item from the cache. Return true if the item was removed.
     *
     * @param string|array $key
     * @return $this
     */
    public function removeCache($key) {
        unset($this->_cache[$this->createCacheKey($key)]);

        return $this;
    }

    /**
     * Set a value to the cache with the defined key.
     * This will overwrite any data with the same key.
     * The value being saved will be returned.
     *
     * @param string|array $key
     * @param mixed $value
     * @return mixed
     */
    public function setCache($key, $value) {
        $this->_cache[$this->createCacheKey($key)] = $value;

        return $value;
    }

    /**
     * Toggle cache on and off.
     *
     * @param bool $on
     * @return $this
     */
    public function toggleCache($on = true) {
        $this->__cacheEnabled = (bool) $on;

        return $this;
    }

}