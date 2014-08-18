<?php
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Io;

use Titon\Io\Exception\ExistingFileException;
use Titon\Io\Exception\InvalidPathException;
use Titon\Io\Exception\MissingFileException;
use Titon\Utility\Path;

/**
 * Shared functionality between file and folder objects.
 *
 * @package Titon\Io
 */
abstract class Node {

    const string OVERWRITE = 'overwrite';
    const string MERGE = 'merge';
    const string SKIP = 'skip';

    /**
     * Parent folder.
     *
     * @type \Titon\Io\Folder
     */
    protected ?Folder $_parent;

    /**
     * Current path.
     *
     * @type string
     */
    protected string $_path = '';

    /**
     * Initialize the file path. If the file doesn't exist, create it.
     *
     * @param string $path
     * @param bool $create
     * @param int $mode
     */
    public function __construct(string $path, bool $create = false, int $mode = 0755) {
        $this->reset($path);

        if ($create) {
            $this->create($mode);
        }
    }

    /**
     * Return the last access time.
     *
     * @return int
     */
    public function accessTime(): int {
        if ($this->exists()) {
            return fileatime($this->path());
        }

        return 0;
    }

    /**
     * Return the last inode change time.
     *
     * @return int
     */
    public function changeTime(): int {
        if ($this->exists()) {
            return filectime($this->path());
        }

        return 0;
    }

    /**
     * Change the group of the file.
     *
     * @param string $group
     * @return bool
     */
    public function chgrp(string $group): bool {
        if (!$this->exists()) {
            return false;
        }

        $path = $this->path();

        $this->reset();

        if (is_link($path)) {
            return lchgrp($path, $group);
        }

        return chgrp($path, $group);
    }

    /**
     * Change the permissions mode of the file.
     *
     * @param int $mode
     * @return bool
     */
    public function chmod(int $mode): bool {
        if (!$this->exists()) {
            return false;
        }

        $this->reset();

        return chmod($this->path(), $mode);
    }

    /**
     * Change the owner of the file.
     *
     * @param string $user
     * @return bool
     */
    public function chown(string $user): bool {
        if (!$this->exists()) {
            return false;
        }

        $path = $this->path();

        $this->reset();

        if (is_link($path)) {
            return lchown($path, $user);
        }

        return chown($path, $user);
    }

    /**
     * Copy the file to a new location and return a new Node object.
     * The functionality of copy will change depending on `$process` and whether the target file exists.
     * This also applies recursively.
     *
     *      overwrite   - Will delete the target file or folder
     *      merge       - Will delete the target file and merge folders
     *      skip        - Will not copy the node if the target exists
     *
     * @param string $target
     * @param string $process
     * @param int $mode
     * @return \Titon\Io\Node
     * @codeCoverageIgnore
     */
    abstract public function copy(string $target, string $process = self::OVERWRITE, int $mode = 0755): ?Node;

    /**
     * Create the file if it doesn't exist.
     *
     * @param int $mode
     * @return bool
     * @codeCoverageIgnore
     */
    abstract public function create(int $mode = 0755): bool;

    /**
     * Remove the file if it exists.
     *
     * @return bool
     * @codeCoverageIgnore
     */
    abstract public function delete(): bool;

    /**
     * Helper method for deleting a file or folder.
     *
     * @param string $path
     * @return bool
     */
    public static function destroy(string $path): bool {
        if (!file_exists($path)) {
            return false;
        }

        return static::load($path)->delete();
    }

    /**
     * Return the parent directory as a string.
     *
     * @return string
     */
    public function dir(): string {
        return dirname($this->path());
    }

    /**
     * Is the file executable.
     *
     * @return bool
     */
    public function executable(): bool {
        return is_executable($this->path());
    }

    /**
     * Check if the file exists.
     *
     * @return bool
     */
    public function exists(): bool {
        return file_exists($this->path());
    }

    /**
     * Return the group name for the file.
     *
     * @return int
     */
    public function group(): int {
        if ($this->exists()) {
            return filegroup($this->path());
        }

        return 0;
    }

    /**
     * Return true if the current path is absolute.
     *
     * @return bool
     */
    public function isAbsolute(): bool {
        return Path::isAbsolute($this->path());
    }

    /**
     * Return true if the current path is relative.
     *
     * @return bool
     */
    public function isRelative(): bool {
        return Path::isRelative($this->path());
    }

    /**
     * Attempt to load a file or folder object at a target location.
     *
     * @param string $path
     * @return \Titon\Io\Node
     * @throws \Titon\Io\Exception\MissingFileException
     */
    public static function load(string $path): Node {
        if (!file_exists($path)) {
            throw new MissingFileException(sprintf('No file or folder found at path: %s', $path));
        }

        if (is_dir($path)) {
            return new Folder($path);
        }

        return new File($path);
    }

    /**
     * Return the last modified time.
     *
     * @return int
     */
    public function modifiedTime(): int {
        if ($this->exists()) {
            return filemtime($this->path());
        }

        return 0;
    }

    /**
     * Move the file to another folder. If a file exists at the target path,
     * either delete the file if `$overwrite` is true, or throw an exception.
     *
     * Use `rename()` to rename the file within the current folder.
     *
     * @param string $target
     * @param bool $overwrite
     * @return bool
     * @throws \Titon\Io\Exception\ExistingFileException
     */
    public function move(string $target, bool $overwrite = true): bool {
        if (!$this->exists()) {
            return false;
        }

        // Don't move if the target exists and overwrite is disabled
        if (file_exists($target)) {
            if ($overwrite) {
                static::destroy($target);
            } else {
                throw new ExistingFileException('Cannot move file as the target already exists');
            }
        }

        // Move folders
        if (rename($this->path(), $target)) {
            $this->reset($target);

            return true;
        }

        return false;
    }

    /**
     * Return the file name.
     *
     * @return string
     */
    public function name(): string {
        return basename($this->path());
    }

    /**
     * Return the owner name for the file.
     *
     * @return int
     */
    public function owner(): int {
        if ($this->exists()) {
            return fileowner($this->path());
        }

        return 0;
    }

    /**
     * Alias for pwd().
     *
     * @return string
     */
    public function path(): string {
        return $this->pwd();
    }

    /**
     * Return the parent folder as a Folder object.
     *
     * @return \Titon\Io\Folder
     */
    public function parent(): ?Folder {
        if ($this->_parent) {
            return $this->_parent;
        }

        $folder = str_replace('\\', '/', dirname($this->path()));

        if ($folder !== '.' && $folder !== '/') {
            $this->_parent = new Folder($folder);
        }

        return $this->_parent;
    }

    /**
     * Return the permissions for the file.
     *
     * @return int
     */
    public function permissions(): int {
        if ($this->exists()) {
            return substr(sprintf('%o', fileperms($this->path())), -4);
        }

        return 0;
    }

    /**
     * Return the current path (print working directory).
     *
     * @return string
     */
    public function pwd(): string {
        return $this->_path;
    }

    /**
     * Is the file readable.
     *
     * @return bool
     */
    public function readable(): bool {
        return is_readable($this->path());
    }

    /**
     * Rename the file within the current folder. If a file exists at the target path,
     * either delete the file if `$overwrite` is true, or throw an exception.
     *
     * Use `move()` to re-locate the file to another folder.
     *
     * @param string $name
     * @param bool $overwrite
     * @return bool
     * @throws \Titon\Io\Exception\ExistingFileException
     */
    public function rename(string $name, bool $overwrite = true): bool {
        if (!$this->exists()) {
            return false;
        }

        // Remove unwanted characters
        $name = preg_replace('/[^_\-\p{Ll}\p{Lm}\p{Lo}\p{Lt}\p{Lu}\p{Nd}]/imu', '-', basename($name));

        // Return early if the same name
        if ($name === $this->name()) {
            return true;
        }

        // Prepend folder
        $target = $this->dir() . '/' . $name;

        // Don't move if the target exists and overwrite is disabled
        if (file_exists($target)) {
            if ($overwrite) {
                static::destroy($target);
            } else {
                throw new ExistingFileException('Cannot rename file as the target already exists');
            }
        }

        // Rename the file within the current folder
        if (rename($this->path(), $target)) {
            $this->reset($target);

            return true;
        }

        return false;
    }

    /**
     * Reset the cache and path.
     *
     * @param string $path
     * @return $this
     */
    public function reset(string $path = ''): this {
        if ($path) {
            // Always use unix style slashes
            $this->_path = str_replace('\\', '/', $path);
        }

        clearstatcache();

        return $this;
    }

    /**
     * Return the current file size.
     *
     * @return int
     * @codeCoverageIgnore
     */
    abstract public function size(): int;

    /**
     * Is the file writable.
     *
     * @return bool
     */
    public function writable(): bool {
        return is_writable($this->path());
    }

}