<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Cache\Storage;

/**
 * A lightweight caching engine that stores data in memory for the duration of the HTTP request.
 *
 * @package Titon\Cache\Storage
 */
class MemoryStorage extends AbstractStorage {

    /**
     * {@inheritdoc}
     */
    public function flush(): bool {
        $this->flushCache();

        return true;
    }

    /**
     * {@inheritdoc}
     */
    public function get(string $key): mixed {
        if ($this->hasCache($key)) {
            return $this->getCache($key);
        }

        return null;
    }

    /**
     * {@inheritdoc}
     */
    public function has(string $key): bool {
        return $this->hasCache($key);
    }

    /**
     * {@inheritdoc}
     */
    public function remove(string $key): bool {
        $this->removeCache($key);

        return true;
    }

    /**
     * {@inheritdoc}
     */
    public function set(string $key, mixed $value, mixed $expires = '+1 day'): bool {
        if ($expires && $this->expires($expires) <= time()) {
            return true;
        }

        $this->setCache($key, $value);

        return true;
    }

}