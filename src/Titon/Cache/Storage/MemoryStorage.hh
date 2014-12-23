<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Cache\Storage;

use Titon\Cache\Exception\MissingItemException;
use Titon\Common\Cacheable;

/**
 * A lightweight caching engine that stores data in memory for the duration of the HTTP request.
 *
 * @package Titon\Cache\Storage
 */
class MemoryStorage extends AbstractStorage {
    use Cacheable;

    /**
     * {@inheritdoc}
     */
    public function flush(): bool {
        $this->flushCache();

        return true;
    }

    /**
     * {@inheritdoc}
     */
    public function get(string $key): mixed {
        if ($this->has($key)) {
            return $this->getCache($key);
        }

        throw new MissingItemException(sprintf('Item with key %s does not exist', $key));
    }

    /**
     * {@inheritdoc}
     */
    public function has(string $key): bool {
        return $this->hasCache($key);
    }

    /**
     * {@inheritdoc}
     */
    public function remove(string $key): bool {
        $this->removeCache($key);

        return true;
    }

    /**
     * {@inheritdoc}
     */
    public function set(string $key, mixed $value, int $expires): bool {
        $this->setCache($key, $value);

        return true;
    }

}