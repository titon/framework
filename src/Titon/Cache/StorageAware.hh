<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Cache;

/**
 * Permits a class to interact with a storage engine within the class layer.
 *
 * @package Titon\Cache
 */
trait StorageAware {

    /**
     * Storage engine instance.
     *
     * @var \Titon\Cache\Storage
     */
    protected ?Storage $storage;

    /**
     * Get the storage engine.
     *
     * @return \Titon\Cache\Storage
     */
    public function getStorage(): ?Storage {
        return $this->storage;
    }

    /**
     * Set the storage engine.
     *
     * @param \Titon\Cache\Storage $storage
     * @return $this
     */
    public function setStorage(Storage $storage): this {
        $this->storage = $storage;

        return $this;
    }

}
