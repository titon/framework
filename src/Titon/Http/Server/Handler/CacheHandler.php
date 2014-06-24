<?php
/**
 * @copyright   2010-2014, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Http\Server\Handler;

use Titon\Cache\Storage;
use Titon\Cache\StorageAware;
use \SessionHandlerInterface;

/**
 * Caches session data using one of the built in cache storage engines.
 *
 * @package Titon\Http\Server\Handler
 * @codeCoverageIgnore
 */
class CacheHandler implements SessionHandlerInterface {
    use StorageAware;

    /**
     * Inject the storage engine.
     *
     * @param \Titon\Cache\Storage $storage
     */
    final public function __construct(Storage $storage) {
        $this->setStorage($storage);
    }

    /**
     * Close the session.
     *
     * @return bool
     */
    public function close() {
        return true;
    }

    /**
     * Triggered when a session is destroyed.
     *
     * @param string $key
     * @return bool
     */
    public function destroy($key) {
        return $this->getStorage()->remove($key);
    }

    /**
     * Triggered when the sessions garbage collector activates.
     *
     * @param int $maxLifetime
     * @return bool
     */
    public function gc($maxLifetime) {
        return $this->getStorage()->flush(time() - $maxLifetime);
    }

    /**
     * Initialize the session.
     *
     * @param string $savePath
     * @param string $sessionId
     * @return bool
     */
    public function open($savePath, $sessionId) {
        return true;
    }

    /**
     * Read value from the session handler.
     *
     * @param string $key
     * @return string
     */
    public function read($key) {
        return (string) $this->getStorage()->get($key);
    }

    /**
     * Write data to the session handler.
     *
     * @param string $key
     * @param mixed $value
     * @return bool
     */
    public function write($key, $value) {
        return $this->getStorage()->set($key, $value);
    }

}
