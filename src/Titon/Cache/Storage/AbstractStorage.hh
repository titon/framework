<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Cache\Storage;

use Titon\Common\Cacheable;
use Titon\Cache\CacheCallback;
use Titon\Cache\Storage;
use Titon\Utility\Time;

/**
 * Primary class for all storage engines to extend. Provides functionality for the Storage interface.
 *
 * @package Titon\Cache\Storage
 */
abstract class AbstractStorage implements Storage {
    use Cacheable;

    /**
     * {@inheritdoc}
     */
    public function decrement(string $key, int $step = 1): ?int {
        $value = $this->get($key);

        if ($value === null) {
            return null;
        }

        $value = (int) $value - $step;

        $this->set($key, $value);

        return $value;
    }

    /**
     * Convert the expires date into a valid UNIX timestamp,
     * or if $ttl is true it will convert to relative seconds.
     *
     * @param mixed $timestamp
     * @param bool $ttl Convert to TTL seconds
     * @return int
     */
    public function expires(mixed $timestamp, bool $ttl = false): int {
        if ($timestamp === 0) {
            // Fall-through

        } else if ($timestamp === null) {
            $timestamp = strtotime('+1 hour');

        } else if (is_string($timestamp)) {
            $timestamp = Time::toUnix($timestamp);

            if ($ttl) {
                $timestamp -= time();
            }
        }

        return (int) $timestamp;
    }

    /**
     * {@inheritdoc}
     */
    public function increment(string $key, int $step = 1): ?int {
        $value = $this->get($key);

        if ($value === null) {
            return null;
        }

        $value = (int) $value + $step;

        $this->set($key, $value);

        return $value;
    }

    /**
     * Rewrite the key to use a specific format.
     *
     * @param string $key
     * @return string
     */
    public function key(string $key): string {
        return $this->cache([__METHOD__, $key], () ==> trim(preg_replace('/[^a-z0-9\-]+/is', '-', $key), '-') );
    }

    /**
     * This method fixes discrepancies with PHP and Hack and its nullable types.
     * Any API method that returns false as failure should return null instead so we can use type hinting.
     *
     * @param mixed $value
     * @return int
     */
    public function returnInt(mixed $value): ?int {
        return ($value === false || $value === null) ? null : (int) $value;
    }

    /**
     * {@inheritdoc}
     */
    public function stats(): Map<string, mixed> {
        return Map {};
    }

    /**
     * {@inheritdoc}
     */
    public function store(string $key, CacheCallback $callback, mixed $expires = '+1 day'): mixed {
        if ($this->has($key)) {
            return $this->get($key);
        }

        $value = call_user_func($callback);

        $this->set($key, $value, $expires);

        return $value;
    }

}