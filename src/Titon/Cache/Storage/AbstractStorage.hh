<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Cache\Storage;

use Titon\Common\Cacheable;
use Titon\Cache\Storage;
use Titon\Utility\Time;
use \Closure;

/**
 * Primary class for all storage engines to extend. Provides functionality for the Storage interface.
 *
 * @package Titon\Cache\Storage
 */
abstract class AbstractStorage implements Storage {
    use Cacheable;

    /**
     * Configuration.
     *
     * @type Map<string, mixed> {
     *      @type string $server    Server(s) to connect and store data in
     *      @type bool $compress    Toggle data compression
     *      @type bool $persistent  Toggle persistent server connections
     *      @type string $expires   Global expiration timer
     *      @type string $prefix    String to prefix before each cache key
     * }
     */
    protected Map<string, mixed> $_config = Map {
        'server' => '127.0.0.1',
        'compress' => false,
        'persistent' => true,
        'expires' => '+1 day',
        'prefix' => ''
    };

    /**
     * {@inheritdoc}
     */
    public function decrement(string $key, int $step = 1): ?int {
        $value = $this->get($key);

        if ($value === null) {
            return null;
        }

        $value -= $step;

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
            return $timestamp;

        } else if ($timestamp === null) {
            $timestamp = strtotime($this->getConfig('expires'));

        } else if (is_string($timestamp)) {
            $timestamp = Time::toUnix($timestamp);

            if ($ttl) {
                $timestamp -= time();
            }
        }

        return $timestamp;
    }

    /**
     * {@inheritdoc}
     */
    public function increment(string $key, int $step = 1): ?int {
        $value = $this->get($key);

        if ($value !== null) {
            $value = $value + $step;

            $this->set($key, $value);

            return $value;
        }

        return null;
    }

    /**
     * Rewrite the key to use a specific format.
     *
     * @param string $key
     * @return string
     */
    public function key(string $key): string {
        return $this->cache([__METHOD__, $key], function(AbstractStorage $self) use ($key) {
            return $self->getConfig('prefix') . trim(preg_replace('/[^a-z0-9\-]+/is', '-', $key), '-');
        });
    }

    /**
     * Parse a server to return host and other values.
     *
     * @param string|array $server
     * @param int $port
     * @param mixed $arg
     * @return Vector<mixed>
     */
    public function parseServer(mixed $server, ?int $port = null, mixed $arg = null): Vector<mixed> {
        if ($server instanceof Traversable) {
            return new Vector($server);
        }

        $parts = explode(':', $server);

        return Vector {
            $parts[0],
            isset($parts[1]) ? $parts[1] : $port,
            isset($parts[2]) ? $parts[2] : $arg
        };
    }

    /**
     * This method fixes discrepancies with PHP and Hack and its nullable types.
     * Any API method that returns false as failure should return null instead so we can use type hinting.
     *
     * @param mixed $value
     * @return mixed
     */
    public function returnValue(mixed $value): mixed {
        return ($value === false) ? null : $value;
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
    public function store(string $key, Closure $callback, mixed $expires = '+1 day'): mixed {
        if ($this->has($key)) {
            return $this->get($key);
        }

        $value = call_user_func($callback);

        $this->set($key, $value, $expires);

        return $value;
    }

}