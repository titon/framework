<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Io;

use Titon\Io\Exception\InvalidPathException;
use Titon\Utility\Path;
use \Exception;
use \GlobIterator;
use \FilesystemIterator;
use \RecursiveDirectoryIterator;
use \RecursiveIteratorIterator;

/**
 * Provides an object interface for interacting with a folder on the file system.
 *
 * @package Titon\Io
 */
class Folder extends Node {

    const ALL = 0;
    const FILES = 1;
    const FOLDERS = 2;

    /**
     * Change directory. Alias for reset().
     *
     * @param string $path
     * @return $this
     */
    public function cd(string $path): this {
        return $this->reset($path);
    }

    /**
     * Change the group of the file.
     * If $recursive is true, set the group on all children.
     *
     * @param int $group
     * @param bool $recursive
     * @return bool
     */
    public function chgrp(int $group, bool $recursive = false): bool {
        if ($recursive) {
            if ($contents = $this->read()) {
                foreach ($contents as $file) {
                    $file->chgrp($group, true);
                }
            }
        }

        return parent::chgrp($group);
    }

    /**
     * Change the permissions mode of the file.
     * If $recursive is true, set the mode on all children.
     *
     * @param int $mode
     * @param bool $recursive
     * @return bool
     */
    public function chmod(int $mode, bool $recursive = false): bool {
        if ($recursive) {
            if ($contents = $this->read()) {
                foreach ($contents as $file) {
                    $file->chmod($mode, true);
                }
            }
        }

        return parent::chmod($mode);
    }

    /**
     * Change the owner of the file.
     * If $recursive is true, set the owner on all children.
     *
     * @param int $user
     * @param bool $recursive
     * @return bool
     */
    public function chown(int $user, bool $recursive = false): bool {
        if ($recursive) {
            if ($contents = $this->read()) {
                foreach ($contents as $file) {
                    $file->chown($user, true);
                }
            }
        }

        return parent::chown($user);
    }

    /**
     * {@inheritdoc}
     */
    public function create(int $mode = 0755): bool {
        if (!$this->exists()) {
            return mkdir($this->path(), $mode, true);
        }

        return false;
    }

    /**
     * {@inheritdoc}
     *
     * @return \Titon\Io\Folder
     */
    public function copy(string $target, int $process = self::MERGE, int $mode = 0755): ?Node {
        if (!$this->exists()) {
            return null;
        }

        // Delete the target folder if overwrite is true
        if ($process === self::OVERWRITE && file_exists($target)) {
            static::destroy($target);
        }

        // Create the target folder and reset folder path
        $destination = new Folder($target, true, $mode);
        $target = $destination->path();

        // Recursively copy over contents to new destination
        if ($contents = $this->read()) {
            foreach ($contents as $file) {
                $to = str_replace($this->path(), $target, $file->path());

                // Skip copy if target exists
                if ($process === self::SKIP && file_exists($to)) {
                    continue;
                }

                // Delete target since File::copy() will throw exception
                if ($process === self::MERGE && file_exists($to) && is_file($to)) {
                    @unlink($to);
                }

                $file->copy($to, $process, $mode);
            }
        }

        return $destination;
    }

    /**
     * {@inheritdoc}
     */
    public function delete(): bool {
        if (!$this->exists()) {
            return false;
        }

        $this->flush()->reset();

        return rmdir($this->path());
    }

    /**
     * Recursively delete all files and folders within this folder.
     *
     * @return $this
     */
    public function flush(): this {
        foreach ($this->read(false, true) as $file) {
            $file->delete();
        }

        return $this;
    }

    /**
     * Scan the folder and return a list of File objects.
     *
     * @param bool $sort
     * @param bool $recursive
     * @return Vector<File>
     */
    public function files(bool $sort = false, bool $recursive = false): Vector<File> {
        // UNSAFE
        // Because read() returns Vector<Node> while we need Vector<File>
        return $this->read($sort, $recursive, self::FILES);
    }

    /**
     * Find all files and folders within the current folder that match a specific pattern.
     *
     * @param string $pattern
     * @return Vector<Node>
     */
    public function find(string $pattern, int $filter = self::ALL): Vector<Node> {
        $contents = Vector {};

        if (!$this->exists()) {
            return $contents;
        }

        try {
            $iterator = new GlobIterator($this->path() . $pattern, FilesystemIterator::SKIP_DOTS | FilesystemIterator::UNIX_PATHS);
        } catch (Exception $e) {
            return $contents;
        }

        /** @var \SPLFileInfo $file */
        foreach ($iterator as $file) {
            if ($file->isDir() && ($filter === self::ALL || $filter === self::FOLDERS)) {
                $contents[] = new Folder($file->getPathname());

            } else if ($file->isFile() && ($filter === self::ALL || $filter === self::FILES)) {
                $contents[] = new File($file->getPathname());
            }
        }

        return $contents;
    }

    /**
     * Scan the folder and return a list of Folder objects.
     *
     * @param bool $sort
     * @param bool $recursive
     * @return Vector<Folder>
     */
    public function folders(bool $sort = false, bool $recursive = false): Vector<Folder> {
        // UNSAFE
        // Because read() returns Vector<Node> while we need Vector<Folder>
        return $this->read($sort, $recursive, self::FOLDERS);
    }

    /**
     * {@inheritdoc}
     */
    public function move(string $target, bool $overwrite = true): bool {
        if (Path::ds($target, true) === Path::ds($this->path(), true)) {
            return true; // Don't move to the same location
        }

        return parent::move($target, $overwrite);
     }

    /**
     * Scan the folder and return a list of File and Folder objects.
     *
     * @param bool $sort
     * @param bool $recursive
     * @param int $filter
     * @return Vector<Node>
     */
    public function read(bool $sort = false, bool $recursive = false, int $filter = self::ALL): Vector<Node> {
        $contents = Vector {};

        if (!$this->exists()) {
            return $contents;
        }

        try {
            $flags = FilesystemIterator::SKIP_DOTS | FilesystemIterator::UNIX_PATHS;

            if ($recursive) {
                $iterator = new RecursiveIteratorIterator(new RecursiveDirectoryIterator($this->path(), $flags), RecursiveIteratorIterator::CHILD_FIRST);
            } else {
                $iterator = new FilesystemIterator($this->path(), $flags);
            }
        } catch (Exception $e) {
            return $contents;
        }

        /** @var \SPLFileInfo $file */
        foreach ($iterator as $file) {
            if ($file->isDir() && ($filter === self::ALL || $filter === self::FOLDERS)) {
                $contents[] = new Folder($file->getPathname());

            } else if ($file->isFile() && ($filter === self::ALL || $filter === self::FILES)) {
                $contents[] = new File($file->getPathname());
            }
        }

        if ($sort) {
            usort($contents, ($a, $b) ==> strcmp($a->path(), $b->path()) );
        }

        return $contents;
    }

    /**
     * {@inheritdoc}
     *
     * @throws \Titon\Io\Exception\InvalidPathException
     */
    public function reset(string $path = ''): this {
        if ($path) {
            if (file_exists($path) && is_file($path)) {
                throw new InvalidPathException(sprintf('Invalid folder path %s, files are not allowed', $path));
            }

            if (substr($path, -1) !== '/') {
                $path .= '/';
            }
        }

        return parent::reset($path);
    }

    /**
     * Return the number of files in the current folder.
     *
     * @return int
     */
    public function size(): int {
        if ($this->exists()) {
            return count($this->read());
        }

        return 0;
    }

}
