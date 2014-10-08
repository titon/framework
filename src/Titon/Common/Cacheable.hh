<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Common;

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
     * @type Map<string, mixed>
     */
    protected Map<string, mixed> $_cache = Map {};

    /**
     * Is cache on or off?
     *
     * @type bool
     */
    private bool $__cacheEnabled = true;

    /**
     * Return all the current cached items.
     *
     * @return Map<string, mixed>
     */
    public function allCache(): Map<string, mixed> {
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
    public function cache(mixed $key, mixed $value): mixed {
        $key = $this->createCacheKey($key);

        if ($cache = $this->getCache($key)) {
            return $cache;
        }

        if (is_callable($value)) {
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
    public function createCacheKey(mixed $keys): string {
        if ($keys instanceof Traversable) {
            $key = '';

            foreach ($keys as $value) {
                if ($value instanceof Traversable) {
                    $key .= '-' . md5(json_encode($value));
                } else if ($value) {
                    $key .= '-' . $value;
                }
            }
        } else {
            $key = $keys;
        }

        return trim((string) $key, '-');
    }

    /**
     * Empty the cache.
     *
     * @return $this
     */
    public function flushCache(): this {
        $this->_cache->clear();

        return $this;
    }

    /**
     * Return a cached item if it exists, else return null.
     *
     * @param string|array $key
     * @return mixed
     */
    public function getCache(mixed $key): mixed {
        if (!$this->__cacheEnabled) {
            return null;
        }

        $key = $this->createCacheKey($key);

        if ($this->hasCache($key)) {
            return $this->_cache->get($key);
        }

        return null;
    }

    /**
     * Check to see if the cache key exists.
     *
     * @param string|array $key
     * @return bool
     */
    public function hasCache(mixed $key): bool {
        return $this->_cache->contains($this->createCacheKey($key));
    }

    /**
     * Remove an item from the cache. Return true if the item was removed.
     *
     * @param string|array $key
     * @return $this
     */
    public function removeCache(mixed $key): this {
        $this->_cache->remove($this->createCacheKey($key));

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
    public function setCache(mixed $key, mixed $value): mixed {
        $this->_cache->set($this->createCacheKey($key), $value);

        return $value;
    }

    /**
     * Toggle cache on and off.
     *
     * @param bool $on
     * @return $this
     */
    public function toggleCache(bool $on = true): this {
        $this->__cacheEnabled = $on;

        return $this;
    }

}