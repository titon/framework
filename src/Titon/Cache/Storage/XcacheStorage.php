<?php
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Cache\Storage;

use Titon\Cache\Exception\MissingExtensionException;

/**
 * A storage engine that uses the Xcache extension for a cache store.
 * This engine can be installed using the Cache::addStorage() method.
 *
 * {{{
 *        new XcacheStorage(array(
 *            'username' => 'admin',
 *            'password' => md5('pass')
 *        ));
 * }}}
 *
 * A sample configuration can be found above, and the following options are available:
 * serialize, compress, username/password (for flush()), expires.
 *
 * @link http://xcache.lighttpd.net
 *
 * @package Titon\Cache\Storage
 */
class XcacheStorage extends AbstractStorage {

    /**
     * Configuration.
     *
     * @type array {
     *      @type string $username  Username used for HTTP auth
     *      @type string $password  Password used for HTTP auth
     * }
     */
    protected $_config = [
        'username' => '',
        'password' => ''
    ];

    /**
     * Validate that APC is installed.
     *
     * @throws \Titon\Cache\Exception\MissingExtensionException
     */
    public function initialize() {
        parent::initialize();

        if (!extension_loaded('xcache')) {
            throw new MissingExtensionException('Xcache extension is not loaded');
        }

        if ($this->getConfig('compress')) {
            ini_set('xcache.optimizer', true);
        }

        ini_set('xcache.admin.user', $this->getConfig('username'));
        ini_set('xcache.admin.pass', $this->getConfig('password'));
    }

    /**
     * {@inheritdoc}
     */
    public function decrement($key, $step = 1) {
        return xcache_dec($this->key($key), $step);
    }

    /**
     * {@inheritdoc}
     */
    public function flush() {
        $backup = [
            'PHP_AUTH_USER' => !empty($_SERVER['PHP_AUTH_USER']) ? $_SERVER['PHP_AUTH_USER'] : '',
            'PHP_AUTH_PW' => !empty($_SERVER['PHP_AUTH_PW']) ? $_SERVER['PHP_AUTH_PW'] : '',
        ];

        $auth = [
            'PHP_AUTH_USER' => $this->getConfig('username'),
            'PHP_AUTH_PW' => $this->getConfig('password')
        ];

        // Set auth
        $_SERVER = $auth + $_SERVER;

        // Clear cache
        $count = xcache_count(XC_TYPE_VAR);

        for ($i = 0; $i < $count; $i++) {
            xcache_clear_cache(XC_TYPE_VAR, $i);
        }

        // Reset auth
        $_SERVER = $backup + $_SERVER;

        return true;
    }

    /**
     * {@inheritdoc}
     */
    public function get($key) {
        return xcache_get($this->key($key));
    }

    /**
     * {@inheritdoc}
     */
    public function has($key) {
        return xcache_isset($this->key($key));
    }

    /**
     * {@inheritdoc}
     */
    public function increment($key, $step = 1) {
        return xcache_inc($this->key($key), $step);
    }

    /**
     * {@inheritdoc}
     */
    public function remove($key) {
        return xcache_unset($this->key($key));
    }

    /**
     * {@inheritdoc}
     */
    public function set($key, $value, $expires = '+1 day') {
        return xcache_set($this->key($key), $value, $this->expires($expires, true));
    }

    /**
     * {@inheritdoc}
     */
    public function stats() {
        $stats = xcache_info(XC_TYPE_VAR, 0);

        return [
            self::HITS => $stats['hits'],
            self::MISSES => $stats['misses'],
            self::UPTIME => false,
            self::MEMORY_USAGE => $stats['size'],
            self::MEMORY_AVAILABLE => $stats['avail']
        ];
    }

}