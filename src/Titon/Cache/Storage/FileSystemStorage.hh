<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Cache\Storage;

use Titon\Io\Exception\MissingFileException;
use Titon\Io\File;
use Titon\Io\Folder;

/**
 * A storage engine that uses the servers local filesystem to store its cached items.
 * This engine can be installed using the Cache::addStorage() method.
 *
 * {{{
 *        new FileSystemStorage([
 *            'prefix' => 'sql_',
 *            'expires' => '+1 day'
 *        ]);
 * }}}
 *
 * A sample configuration can be found above, and the following options are available: prefix, expires.
 *
 * @package Titon\Cache\Storage
 */
class FileSystemStorage extends AbstractStorage {

    /**
     * Configuration.
     *
     * @type Map<string, mixed> {
     *      @type string $directory Temporary directory to store files
     * }
     */
    protected Map<string, mixed> $_config = Map {
        'directory' => ''
    };

    /**
     * List of cache expiration times via File objects.
     *
     * @type Map<string, File>
     */
    protected Map<string, File> $_expires = Map {};

    /**
     * List of cache File objects.
     *
     * @type Map<string, File>
     */
    protected Map<string, File> $_files = Map {};

    /**
     * Folder object for the cache folder.
     *
     * @type \Titon\Io\Folder
     */
    protected Folder $_folder;

    /**
     * Allow path to be set through constructor.
     *
     * @param string|Map $path
     */
    public function __construct(mixed $path) {
        if (is_string($path)) {
            $path = Map {'directory' => $path};
        }

        parent::__construct($path);
    }

    /**
     * Always use serialization with file system caching.
     *
     * @uses Titon\Utility\Path
     *
     * @throws \Titon\Io\Exception\MissingFileException
     */
    public function initialize(): void {
        parent::initialize();

        $path = $this->getConfig('directory');

        if (!$path) {
            throw new MissingFileException('Temporary storage directory not defined');
        }

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
        if (isset($this->_files[$key])) {
            return $this->_files[$key];
        }

        return $this->_files[$key] = new File($this->_getPath($key), true);
    }

    /**
     * {@inheritdoc}
     */
    public function loadExpires(string $key): File {
        if (isset($this->_expires[$key])) {
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

        unset($this->_expires[$key], $this->_files[$key]);

        return true;
    }

    /**
     * {@inheritdoc}
     */
    public function set(string $key, mixed $value, mixed $expires = '+1 day'): bool {
        return (
            $this->loadCache($key)->write(serialize($value)) &&
            $this->loadExpires($key)->write($this->expires($expires))
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
        return $this->_folder->path() . $this->key($key) . '.' . $ext;
    }

}