<?hh //strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Cache\Storage;

use Titon\Common\Base;
use Titon\Common\Cacheable;
use Titon\Cache\Storage;
use Titon\Utility\Time;
use \Closure;

/**
 * Primary class for all storage engines to extend. Provides functionality from the Base class and the Storage interface.
 *
 * @package Titon\Cache\Storage
 */
abstract class AbstractStorage extends Base implements Storage {
    use Cacheable;

    /**
     * Configuration.
     *
     * @type array {
     *      @type string $server    Server(s) to connect and store data in
     *      @type bool $compress    Toggle data compression
     *      @type bool $persistent  Toggle persistent server connections
     *      @type string $expires   Global expiration timer
     *      @type string $prefix    String to prefix before each cache key
     * }
     */
    protected $_config = [
        'server' => '127.0.0.1',
        'compress' => false,
        'persistent' => true,
        'expires' => '+1 day',
        'prefix' => ''
    ];

    /**
     * {@inheritdoc}
     */
    public function decrement($key, $step = 1) {
        $value = $this->get($key);

        if ($value === null) {
            return false;
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
    public function expires($timestamp, $ttl = false) {
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
    public function increment($key, $step = 1) {
        $value = $this->get($key);

        if ($value !== null) {
            $value = $value + $step;

            $this->set($key, $value);

            return $value;
        }

        return false;
    }

    /**
     * Rewrite the key to use a specific format.
     *
     * @param string $key
     * @return string
     */
    public function key($key) {
        $prefix = $this->getConfig('prefix');

        return $this->cache([__METHOD__, $key], function(AbstractStorage $self) use ($prefix, $key) {
            return $prefix . trim(preg_replace('/[^a-z0-9\.]+/is', '', str_replace(['\\', '::', '/', '-', '_'], '.', $self->createCacheKey($key))), '.');
        });
    }

    /**
     * Parse a server to return host and other values.
     *
     * @param string|array $server
     * @param int $port
     * @param mixed $arg
     * @return array
     */
    public function parseServer($server, $port, $arg = null) {
        if (is_array($server)) {
            return $server;
        }

        $parts = explode(':', $server);

        return [
            $parts[0],
            isset($parts[1]) ? $parts[1] : $port,
            isset($parts[2]) ? $parts[2] : $arg
        ];
    }

    /**
     * {@inheritdoc}
     */
    public function stats() {
        return [];
    }

    /**
     * {@inheritdoc}
     */
    public function store($key, Closure $callback, $expires = '+1 day') {
        if ($this->has($key)) {
            return $this->get($key);
        }

        $value = call_user_func($callback);

        $this->set($key, $value, $expires);

        return $value;
    }

}