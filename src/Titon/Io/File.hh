<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Io;

use Titon\Io\Exception\ExistingFileException;
use Titon\Io\Exception\InvalidPathException;

/**
 * Provides an object interface for interacting with a file on the file system.
 * Encapsulates methods for opening, reading, writing, deleting, copying, etc.
 *
 * @package Titon\Io
 */
class File extends Node {

    /**
     * Resource handle.
     *
     * @var resource
     */
    protected ?resource $handle;

    /**
     * Current read / write mode.
     *
     * @var string
     */
    protected string $mode = '';

    /**
     * Close the current file resource handler when object is destroyed.
     */
    public function __destruct(): void {
        $this->close();
    }

    /**
     * Append data to the end of a file.
     *
     * @param string $data
     * @return bool
     */
    public function append(string $data): bool {
        return $this->write($data, 'a', false);
    }

    /**
     * Close the current file resource handler.
     *
     * @return bool
     */
    public function close(): bool {
        if (is_resource($this->handle)) {
            $this->unlock();

            return fclose($this->handle);
        }

        return false;
    }

    /**
     * {@inheritdoc}
     *
     * @throws \Titon\Io\Exception\ExistingFileException
     */
    public function copy(string $target, int $process = self::OVERWRITE, int $mode = 0755): ?File {
        if (!$this->exists()) {
            return null;
        }

        if (file_exists($target) && $process !== self::OVERWRITE) {
            throw new ExistingFileException('Cannot copy file as the target already exists');
        }

        if (copy($this->path(), $target)) {
            $file = new File($target);
            $file->chmod($mode);

            return $file;
        }

        return null;
    }

    /**
     * {@inheritdoc}
     */
    public function create(int $mode = 0755): bool {
        $folder = $this->parent();

        if (!$folder) {
            return false;
        }

        if (!$folder->exists()) {
            $folder->create();
        }

        if (!$this->exists() && $folder->writable()) {
            if (touch($this->path())) {
                if ($mode) {
                    $this->chmod($mode);
                }

                return true;
            }
        }

        return false;
    }

    /**
     * Remove the file if it exists.
     *
     * @return bool
     */
    public function delete(): bool {
        if ($this->exists()) {
            $this->close();
            $this->reset();

            return @unlink($this->path());
        }

        return false;
    }

    /**
     * Return the file extension.
     *
     * @return string
     */
    public function ext(): string {
        return strtolower(pathinfo($this->path(), PATHINFO_EXTENSION));
    }

    /**
     * Lock a file for reading or writing.
     *
     * @param int $mode
     * @return bool
     */
    public function lock(int $mode = LOCK_SH): bool {
        if (is_resource($this->handle)) {
            return flock($this->handle, $mode);
        }

        return false;
    }

    /**
     * Return an MD5 checksum of the file.
     *
     * @param bool $raw
     * @return string
     */
    public function md5(bool $raw = false): string {
        if ($this->exists()) {
            return md5_file($this->path(), $raw);
        }

        return '';
    }

    /**
     * Return the mime type for the file.
     *
     * @return string
     */
    public function mimeType(): string {
        if (!$this->exists()) {
            return '';
        }

        $type = shell_exec(sprintf("file -b --mime %s", escapeshellarg($this->path())));

        if ($type && strpos($type, ';') !== false) {
            $type = strstr($type, ';', true);
        }

        // Fallback because of fileinfo bug: https://bugs.php.net/bug.php?id=53035
        if (!$type) {
            $info = finfo_open(FILEINFO_MIME_TYPE);
            $type = finfo_file($info, $this->path());
            finfo_close($info);
        }

        return $type;
    }

    /**
     * Open a file resource handler for reading and writing.
     *
     * @param string $mode
     * @return bool
     */
    public function open(string $mode): bool {
        if (!$this->exists()) {
            return false;
        }

        if (is_resource($this->handle)) {
            if ($mode === $this->mode) {
                return true;
            } else {
                $this->close();
            }
        }

        $this->reset();

        $this->handle = fopen($this->path(), $mode);
        $this->mode = $mode;

        return is_resource($this->handle);
    }

    /**
     * Prepend data to the beginning of a file.
     *
     * @param string $data
     * @return bool
     */
    public function prepend(string $data): bool {
        $content = $this->read();

        return $this->write($data . $content, 'w', false);
    }

    /**
     * Open a file for reading. If $length is provided, will only read up to that limit.
     *
     * @param int $length
     * @param string $mode
     * @return string
     */
    public function read(int $length = -1, string $mode = 'rb'): string {
        if (!$this->open($mode)) {
            return '';
        }

        if ($this->lock()) {
            if ($length === -1) {
                $length = $this->size() ?: 1;
            }

            $content = fread($this->handle, $length);

            $this->close();

            return $content;
        }

        return '';
    }

    /**
     * Reset the cache and path.
     *
     * @param string $path
     * @return $this
     * @throws \Titon\Io\Exception\InvalidPathException
     */
    public function reset(string $path = ''): this {
        if ($path && file_exists($path) && is_dir($path)) {
            throw new InvalidPathException(sprintf('Invalid file path %s, folders are not allowed', $path));
        }

        return parent::reset($path);
    }

    /**
     * Return the current file size.
     *
     * @return int
     */
    public function size(): int {
        if ($this->exists()) {
            return filesize($this->path());
        }

        return 0;
    }

    /**
     * Unlock a file for reading or writing.
     *
     * @return bool
     */
    public function unlock(): bool {
        if (is_resource($this->handle)) {
            return flock($this->handle, LOCK_UN);
        }

        return false;
    }

    /**
     * Write data to a file (will erase any previous contents).
     *
     * @param string $data
     * @param string $mode
     * @param bool $close
     * @return bool
     */
    public function write(string $data, string $mode = 'w', bool $close = true): bool {
        if (!$this->open($mode)) {
            return false;
        }

        if ($this->lock(LOCK_EX)) {
            $result = fwrite($this->handle, $data);

            $this->unlock();

            if ($close) {
                $this->close();
            }

            return (bool) $result;
        }

        return false;
    }

}
