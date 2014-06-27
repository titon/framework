<?php
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
    public function flush() {
        return $this->flushCache();
    }

    /**
     * {@inheritdoc}
     */
    public function get($key) {
        if ($this->hasCache($key)) {
            return $this->getCache($key);
        }

        return null;
    }

    /**
     * {@inheritdoc}
     */
    public function has($key) {
        return $this->hasCache($key);
    }

    /**
     * {@inheritdoc}
     */
    public function remove($key) {
        return $this->removeCache($key);
    }

    /**
     * {@inheritdoc}
     */
    public function set($key, $value, $expires = '+1 day') {
        if ($expires && $this->expires($expires) <= time()) {
            return true;
        }

        $this->setCache($key, $value);

        return true;
    }

}