<?php
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Cache\Storage;

use Titon\Cache\Exception\MissingExtensionException;

/**
 * A storage engine that uses the Wincache extension for a cache store; available on Windows platforms.
 * This engine can be installed using the Cache::addStorage() method. No configuration options are available for this engine.
 *
 * @link http://php.net/manual/book.wincache.php
 *
 * @package Titon\Cache\Storage
 */
class WincacheStorage extends AbstractStorage {

    /**
     * Validate that Wincache is installed.
     *
     * @throws \Titon\Cache\Exception\MissingExtensionException
     */
    public function initialize() {
        parent::initialize();

        if (!extension_loaded('wincache')) {
            throw new MissingExtensionException('Wincache extension is not loaded');
        }
    }

    /**
     * {@inheritdoc}
     */
    public function decrement($key, $step = 1) {
        return wincache_ucache_dec($this->key($key), $step);
    }

    /**
     * {@inheritdoc}
     */
    public function flush() {
        return wincache_ucache_clear();
    }

    /**
     * {@inheritdoc}
     */
    public function get($key) {
        $value = wincache_ucache_get($this->key($key), $success);

        if (!$success) {
            return null;
        }

        return $value;
    }

    /**
     * {@inheritdoc}
     */
    public function has($key) {
        return wincache_ucache_exists($this->key($key));
    }

    /**
     * {@inheritdoc}
     */
    public function increment($key, $step = 1) {
        return wincache_ucache_inc($this->key($key), $step);
    }

    /**
     * {@inheritdoc}
     */
    public function remove($key) {
        return wincache_ucache_delete($this->key($key));
    }

    /**
     * {@inheritdoc}
     */
    public function set($key, $value, $expires = '+1 day') {
        return wincache_ucache_set($this->key($key), $value, $this->expires($expires, true));
    }

    /**
     * {@inheritdoc}
     */
    public function stats() {
        $stats = wincache_ucache_info();
        $memory = wincache_ucache_meminfo();

        return [
            self::HITS => $stats['total_hit_count'],
            self::MISSES => $stats['total_miss_count'],
            self::UPTIME => $stats['total_cache_uptime'],
            self::MEMORY_USAGE => $memory['memory_total'],
            self::MEMORY_AVAILABLE => $memory['memory_free']
        ];
    }

}