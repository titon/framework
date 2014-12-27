<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Cache\Storage;

use Titon\Cache\Exception\MissingItemException;
use Titon\Cache\StatsMap;
use Titon\Common\Exception\MissingExtensionException;

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
     * @throws \Titon\Common\Exception\MissingExtensionException
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
        $success = true;
        $value = apc_fetch($key, $success);

        if ($value === false && $success === false) {
            throw new MissingItemException(sprintf('Item with key %s does not exist', $key));
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

        $get = function(string $key, array<string, mixed> $data): mixed {
            return array_key_exists($key, $data) ? $data[$key] : 0;
        };

        return Map {
            self::HITS => $get('num_hits', $stats),
            self::MISSES => $get('num_misses', $stats),
            self::UPTIME => $get('start_time', $stats),
            self::MEMORY_USAGE => $get('mem_size', $stats),
            self::MEMORY_AVAILABLE => $get('avail_mem', $info)
        };
    }

}