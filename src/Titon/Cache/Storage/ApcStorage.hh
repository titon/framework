<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Cache\Storage;

use Titon\Cache\Exception\MissingExtensionException;

/**
 * A storage engine that uses the APC extension for a cache store; requires pecl/apc.
 * This engine can be installed using the Cache::addStorage() method. No configuration options are available for this engine.
 *
 * @link http://pecl.php.net/package/apc
 *
 * @package Titon\Cache\Storage
 */
class ApcStorage extends AbstractStorage {

    /**
     * Validate that APC is installed.
     *
     * @throws \Titon\Cache\Exception\MissingExtensionException
     */
    public function initialize(): void {
        parent::initialize();

        if (!extension_loaded('apc')) {
            throw new MissingExtensionException('APC extension is not loaded');
        }
    }

    /**
     * {@inheritdoc}
     */
    public function decrement(string $key, int $step = 1): ?int {
        return $this->returnValue(apc_dec($this->key($key), $step));
    }

    /**
     * {@inheritdoc}
     */
    public function flush(): bool {
        return (apc_clear_cache() && apc_clear_cache('user'));
    }

    /**
     * {@inheritdoc}
     */
    public function get(string $key): mixed {
        return $this->returnValue(apc_fetch($this->key($key)));
    }

    /**
     * {@inheritdoc}
     */
    public function has(string $key): bool {
        return apc_exists($this->key($key));
    }

    /**
     * {@inheritdoc}
     */
    public function increment(string $key, int $step = 1): ?int {
        return $this->returnValue(apc_inc($this->key($key), $step));
    }

    /**
     * {@inheritdoc}
     */
    public function remove(string $key): bool {
        return apc_delete($this->key($key));
    }

    /**
     * {@inheritdoc}
     */
    public function set(string $key, mixed $value, mixed $expires = '+1 day'): bool {
        $expires = $this->expires($expires, true);

        // Immediately invalidate
        if ($expires < 0) {
            return true;
        }

        return apc_store($this->key($key), $value, $expires);
    }

    /**
     * {@inheritdoc}
     */
    public function stats(): Map<string, mixed> {
        $stats = apc_cache_info();
        $info = apc_sma_info();

        if ($stats === false) {
            return Map {};
        }

        // HHVM needs index checks as it doesn't return all keys
        return Map {
            self::HITS => isset($stats['num_hits']) ? $stats['num_hits'] : 0,
            self::MISSES => isset($stats['num_misses']) ? $stats['num_misses'] : 0,
            self::UPTIME => isset($stats['start_time']) ? $stats['start_time'] : 0,
            self::MEMORY_USAGE => isset($stats['mem_size']) ? $stats['mem_size'] : 0,
            self::MEMORY_AVAILABLE => isset($info['avail_mem']) ? $info['avail_mem'] : 0
        };
    }

}