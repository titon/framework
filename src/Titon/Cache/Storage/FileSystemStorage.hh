<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Cache\Storage;

use Titon\Io\File;
use Titon\Io\Folder;

type FileMap = Map<string, File>;

/**
 * A storage engine that uses the servers local filesystem to store its cached items.
 * This engine can be installed using the Cache::addStorage() method.
 *
 * {{{
 *        new FileSystemStorage('/path/to/cache/');
 * }}}
 *
 * @package Titon\Cache\Storage
 */
class FileSystemStorage extends AbstractStorage {

    /**
     * List of cache expiration times via File objects.
     *
     * @type \Titon\Cache\FileMap
     */
    protected FileMap $_expires = Map {};

    /**
     * List of cache File objects.
     *
     * @type \Titon\Cache\FileMap
     */
    protected FileMap $_files = Map {};

    /**
     * Folder object for the cache folder.
     *
     * @type \Titon\Io\Folder
     */
    protected Folder $_folder;

    /**
     * Allow path to be set through constructor.
     *
     * @param string $path
     */
    public function __construct(string $path) {
        $this->_folder = new Folder($path, true);
    }

    /**
     * {@inheritdoc}
     */
    public function flush(): bool {
        foreach ($this->_files as $key => $file) {
            $file->delete();
            $this->_expires[$key]->delete();
        }

        $this->_files->clear();
        $this->_expires->clear();;

        clearstatcache();

        return true;
    }

    /**
     * {@inheritdoc}
     */
    public function get(string $key): mixed {
        if ($this->has($key)) {
            return unserialize($this->loadCache($key)->read());
        }

        return null;
    }

    /**
     * {@inheritdoc}
     */
    public function has(string $key): bool {
        if (file_exists($this->_getPath($key, 'expires'))) {
            $expires = $this->loadExpires($key)->read();

            if ($expires && $expires >= time()) {
                return true;
            } else {
                $this->remove($key);
            }
        }

        return false;
    }

    /**
     * {@inheritdoc}
     */
    public function loadCache(string $key): File {
        if ($this->_files->contains($key)) {
            return $this->_files[$key];
        }

        return $this->_files[$key] = new File($this->_getPath($key), true);
    }

    /**
     * {@inheritdoc}
     */
    public function loadExpires(string $key): File {
        if ($this->_expires->contains($key)) {
            return $this->_expires[$key];
        }

        return $this->_expires[$key] = new File($this->_getPath($key, 'expires'), true);
    }

    /**
     * {@inheritdoc}
     */
    public function remove(string $key): bool {
        $this->loadCache($key)->delete();
        $this->loadExpires($key)->delete();

        $this->_expires->remove($key);
        $this->_files->remove($key);

        return true;
    }

    /**
     * {@inheritdoc}
     */
    public function set(string $key, mixed $value, int $expires): bool {
        return (
            $this->loadCache($key)->write(serialize($value)) &&
            $this->loadExpires($key)->write($expires)
        );
    }

    /**
     * Return the file system path for a cache key.
     *
     * @param string $key
     * @param string $ext
     * @return string
     */
    protected function _getPath(string $key, string $ext = 'cache'): string {
        $key = trim(preg_replace('/[^a-z0-9\-]+/is', '-', $key), '-');

        return $this->_folder->path() . $key . '.' . $ext;
    }

}