<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Cache\Storage;

use Titon\Cache\Exception\MissingItemException;
use Titon\Io\Exception\InvalidPathException;
use Titon\Io\File;
use Titon\Io\Folder;

type FileCache = shape('expires' => int, 'data' => string);
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
     * List of cached file objects for the current request.
     * Does not include files that already exist.
     *
     * @var \Titon\Cache\FileMap
     */
    protected FileMap $_files = Map {};

    /**
     * Folder object for the cache folder.
     *
     * @var \Titon\Io\Folder
     */
    protected Folder $_folder;

    /**
     * Set path through constructor.
     *
     * @param string $path
     */
    public function __construct(string $path) {
        if (!$path) {
            throw new InvalidPathException(sprintf('Cache directory %s does not exist', $path));
        }

        $this->_folder = new Folder($path, true);
    }

    /**
     * {@inheritdoc}
     */
    public function flush(): bool {
        $this->_files->clear();
        $this->_folder->flush();

        clearstatcache();

        return true;
    }

    /**
     * {@inheritdoc}
     */
    public function get(string $key): mixed {
        if ($this->has($key)) {
            return unserialize($this->_readCache($key)['data']);
        }

        throw new MissingItemException(sprintf('Item with key %s does not exist', $key));
    }

    /**
     * {@inheritdoc}
     */
    public function has(string $key): bool {
        if (file_exists($this->_buildPath($key))) {
            if ($this->_readCache($key)['expires'] >= time()) {
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
    public function remove(string $key): bool {
        $this->_loadCache($key)->delete();
        $this->_files->remove($key);

        return true;
    }

    /**
     * {@inheritdoc}
     */
    public function set(string $key, mixed $value, int $expires): bool {
        return $this->_loadCache($key)->write($expires . "\n" . serialize($value));
    }

    /**
     * Build an absolute path to the cache on the file system using the defined key.
     *
     * @param string $key
     * @return string
     */
    <<__Memoize>>
    protected function _buildPath(string $key): string {
        $path = trim(preg_replace('/[^a-z0-9\-]+/is', '-', $key), '-');
        $path = $this->_folder->path() . $path . '.cache';

        return $path;
    }

    /**
     * Attempt to load a cache from the file system. If the cache does not exist, create it.
     *
     * @param string $key
     * @return \Titon\Io\File
     */
    protected function _loadCache(string $key): File {
        if ($this->_files->contains($key)) {
            return $this->_files[$key];
        }

        return $this->_files[$key] = new File($this->_buildPath($key), true);
    }

    /**
     * Load the cached file and return the expires timestamp and serialized data.
     *
     * @param string $key
     * @return \Titon\Cache\Storage\FileCache
     */
    protected function _readCache(string $key): FileCache {
        return $this->_splitCache($this->_loadCache($key)->read());
    }

    /**
     * Split the cache into 2 separate parts, the expires timestamp, and the serialized data.
     *
     * @param string $cache
     * @return \Titon\Cache\Storage\FileCache
     */
    protected function _splitCache(string $cache): FileCache {
        list($expires, $data) = explode("\n", $cache, 2);

        return shape(
            'expires' => (int) $expires,
            'data' => $data
        );
    }

}