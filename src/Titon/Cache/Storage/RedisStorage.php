<?php
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Cache\Storage;

use Titon\Cache\Exception\AuthenticateFailureException;
use Titon\Cache\Exception\InvalidServerException;
use Titon\Cache\Exception\MissingExtensionException;
use \Redis;

/**
 * A storage engine for the Redis key-value store; requires the redis modules.
 * This engine can be installed using the Cache::addStorage() method.
 *
 * {{{
 *        new RedisStorage(array(
 *            'server' => 'localhost:11211',
 *            'persistent' => true
 *        ));
 * }}}
 *
 * A sample configuration can be found above, and the following options are available:
 * server, persistent, serialize, expires.
 *
 * @link https://github.com/nicolasff/phpredis
 *
 * @package Titon\Cache\Storage
 */
class RedisStorage extends AbstractStorage {

    /**
     * Default Redis server port.
     */
    const PORT = 6379;

    /**
     * The third-party class instance.
     *
     * @type \Redis
     */
    public $connection;

    /**
     * Configuration.
     *
     * @type array {
     *      @type string $password  Password to authenticate with
     * }
     */
    protected $_config = [
        'password' => ''
    ];

    /**
     * Initialize the Redis instance and set all relevant options.
     *
     * @throws \Titon\Cache\Exception\MissingExtensionException
     * @throws \Titon\Cache\Exception\InvalidServerException
     * @throws \Titon\Cache\Exception\AuthenticateFailureException
     */
    public function initialize() {
        parent::initialize();

        if (!extension_loaded('redis')) {
            throw new MissingExtensionException('Redis extension is not loaded');
        }

        $config = $this->allConfig();

        if (!$config['server']) {
            throw new InvalidServerException(sprintf('No server has been defined for %s', $this->inform('className')));
        }

        $this->connection = new Redis();

        if (extension_loaded('igbinary')) {
            $this->connection->setOption(Redis::OPT_SERIALIZER, Redis::SERIALIZER_IGBINARY);
        }

        list($host, $port, $timeout) = $this->parseServer($config['server'], self::PORT, 0);

        if ($config['persistent']) {
            $this->connection->pconnect($host, $port, $timeout);
        } else {
            $this->connection->connect($host, $port, $timeout);
        }

        if ($config['password']) {
            if (!$this->connection->auth($config['password'])) {
                throw new AuthenticateFailureException('Could not authenticate with Redis');
            }
        }
    }

    /**
     * {@inheritdoc}
     */
    public function decrement($key, $step = 1) {
        return $this->connection->decrBy($this->key($key), $step);
    }

    /**
     * {@inheritdoc}
     */
    public function flush() {
        return $this->connection->flushDB();
    }

    /**
     * {@inheritdoc}
     */
    public function get($key) {
        $value = $this->connection->get($this->key($key));

        if ($value === false) {
            return null;
        }

        return $value;
    }

    /**
     * {@inheritdoc}
     */
    public function has($key) {
        return $this->connection->exists($this->key($key));
    }

    /**
     * {@inheritdoc}
     */
    public function increment($key, $step = 1) {
        return $this->connection->incrBy($this->key($key), $step);
    }

    /**
     * {@inheritdoc}
     */
    public function remove($key) {
        $this->connection->delete($this->key($key));

        return true;
    }

    /**
     * {@inheritdoc}
     */
    public function set($key, $value, $expires = '+1 day') {
        if ($expires === 0) {
            return $this->connection->set($this->key($key), $value);
        }

        $expires = $this->expires($expires, true);

        // Immediately invalidate
        if ($expires < 0) {
            return true;
        }

        return $this->connection->setex($this->key($key), $expires, $value);
    }

    /**
     * {@inheritdoc}
     */
    public function stats() {
        $stats = $this->connection->info();

        return [
            self::HITS => false,
            self::MISSES => false,
            self::UPTIME => $stats['uptime_in_seconds'],
            self::MEMORY_USAGE => $stats['used_memory'],
            self::MEMORY_AVAILABLE => false
        ];
    }

}