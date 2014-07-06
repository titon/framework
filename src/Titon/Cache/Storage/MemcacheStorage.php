<?hh //strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Cache\Storage;

use Titon\Cache\Exception\InvalidServerException;
use Titon\Cache\Exception\MissingExtensionException;
use \Memcached;

/**
 * A storage engine for the Memcache key-value store; requires pecl/memcached.
 * This engine can be installed using the Cache::addStorage() method.
 *
 * {{{
 *        new MemcacheStorage(array(
 *            'server' => 'localhost:11211',
 *            'persistent' => true,
 *            'compress' => true
 *        ));
 * }}}
 *
 * A sample configuration can be found above, and the following options are available:
 * server (array or string), compress, persistent, serialize, expires, prefix.
 *
 * @link http://pecl.php.net/package/memcached
 *
 * @package Titon\Cache\Storage
 */
class MemcacheStorage extends AbstractStorage {

    /**
     * Default Memcache server port.
     */
    const PORT = 11211;

    /**
     * Default Memcache server weight.
     */
    const WEIGHT = 100;

    /**
     * The third-party class instance.
     *
     * @type \Memcached
     */
    public $connection;

    /**
     * Configuration.
     *
     * @type array {
     *      @type string $id    Unique ID used for persistence
     *      @type bool $buffer  Buffer writes until data is retrieved
     * }
     */
    protected $_config = [
        'id' => '',
        'buffer' => false
    ];

    /**
     * Initialize the Memcached instance and set all relevant options.
     *
     * @throws \Titon\Cache\Exception\MissingExtensionException
     * @throws \Titon\Cache\Exception\InvalidServerException
     */
    public function initialize() {
        parent::initialize();

        if (!extension_loaded('memcached')) {
            throw new MissingExtensionException('Memcache extension is not loaded');
        }

        $config = $this->allConfig();

        if (!$config['server']) {
            throw new InvalidServerException(sprintf('No server has been defined for %s', $this->inform('className')));
        }

        if ($config['persistent'] && !$config['id']) {
            $config['id'] = 'titon';
        }

        $this->connection = new Memcached($config['id']);

        $serverList = $this->connection->getServerList();

        // Only add servers if none exist
        if (empty($serverList)) {
            $this->connection->setOption(Memcached::OPT_COMPRESSION, (bool) $config['compress']);
            $this->connection->setOption(Memcached::OPT_LIBKETAMA_COMPATIBLE, true);
            $this->connection->setOption(Memcached::OPT_BUFFER_WRITES, (bool) $config['buffer']);

            if ($config['persistent']) {
                $this->connection->setOption(Memcached::OPT_DISTRIBUTION, Memcached::DISTRIBUTION_CONSISTENT);
            }

            if (extension_loaded('igbinary')) {
                $this->connection->setOption(Memcached::OPT_SERIALIZER, Memcached::SERIALIZER_IGBINARY);
            }

            if (is_array($config['server'])) {
                $this->connection->addServers(array_map([$this, 'parseServer'], $config['server']));

            } else {
                list($host, $port, $weight) = $this->parseServer($config['server'], self::PORT, self::WEIGHT);

                $this->connection->addServer($host, $port, $weight);
            }
        }
    }

    /**
     * {@inheritdoc}
     */
    public function decrement($key, $step = 1) {
        return ($this->connection->decrement($this->key($key), $step) !== false);
    }

    /**
     * {@inheritdoc}
     */
    public function flush() {
        return $this->connection->flush();
    }

    /**
     * {@inheritdoc}
     */
    public function get($key) {
        $value = $this->connection->get($this->key($key));

        if (!$value && $this->connection->getResultCode() === Memcached::RES_NOTFOUND) {
            return null;
        }

        return $value;
    }

    /**
     * {@inheritdoc}
     */
    public function has($key) {
        return (bool) ($this->get($key) && $this->connection->getResultCode() === Memcached::RES_SUCCESS);
    }

    /**
     * {@inheritdoc}
     */
    public function increment($key, $step = 1) {
        return ($this->connection->increment($this->key($key), $step) !== false);
    }

    /**
     * {@inheritdoc}
     */
    public function remove($key) {
        return $this->connection->delete($this->key($key));
    }

    /**
     * {@inheritdoc}
     */
    public function set($key, $value, $expires = '+1 day') {
        return $this->connection->set($this->key($key), $value, $this->expires($expires));
    }

    /**
     * {@inheritdoc}
     */
    public function stats() {
        $servers = $this->connection->getServerList();
        $stats = $this->connection->getStats();
        $stats = $stats[$servers[0]['host'] . ':' . $servers[0]['port']];

        return [
            self::HITS => $stats['get_hits'],
            self::MISSES => $stats['get_misses'],
            self::UPTIME => $stats['uptime'],
            self::MEMORY_USAGE => $stats['bytes'],
            self::MEMORY_AVAILABLE => $stats['limit_maxbytes']
        ];
    }

}