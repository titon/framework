<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Cache\Storage;

use Titon\Cache\Exception\MissingItemException;
use Titon\Cache\Exception\MissingExtensionException;
use Titon\Cache\StatsMap;

/**
 * A storage engine that uses the APC extension for a cache store; requires pecl/apc.
 * This engine can be installed using the Cache::addStorage() method.
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
    public function __construct() {
        if (!extension_loaded('apc')) {
            throw new MissingExtensionException('APC extension is not loaded');
        }
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
        $value = apc_fetch($key, $success);

        if ($value === false && $success === false) {
            throw new MissingItemException(sprintf('Item with key %s does not exist.', $key));
        }

        return $value;
    }

    /**
     * {@inheritdoc}
     */
    public function has(string $key): bool {
        return apc_exists($key);
    }

    /**
     * {@inheritdoc}
     */
    public function remove(string $key): bool {
        return apc_delete($key);
    }

    /**
     * {@inheritdoc}
     */
    public function set(string $key, mixed $value, int $expires): bool {
        return apc_store($key, $value, $expires - time()); // APC uses TTL
    }

    /**
     * {@inheritdoc}
     */
    public function stats(): StatsMap {
        $stats = apc_cache_info();
        $info = apc_sma_info();

        if ($stats === false) {
            return Map {};
        }

        return Map {
            self::HITS => array_key_exists('num_hits', $stats) ? $stats['num_hits'] : 0,
            self::MISSES => array_key_exists('num_misses', $stats) ? $stats['num_misses'] : 0,
            self::UPTIME => array_key_exists('start_time', $stats) ? $stats['start_time'] : 0,
            self::MEMORY_USAGE => array_key_exists('mem_size', $stats) ? $stats['mem_size'] : 0,
            self::MEMORY_AVAILABLE => array_key_exists('avail_mem', $info) ? $info['avail_mem'] : 0
        };
    }

}