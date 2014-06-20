<?php
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Utility;

use Titon\Utility\Exception\InvalidArgumentException;
use Titon\Utility\Exception\InvalidTypeException;

/**
 * Provides convenience functions for inflecting notation paths, namespace paths and file system paths.
 *
 * @package Titon\Utility
 */
class Path extends Macro {

    /**
     * Directory separator.
     */
    const SEPARATOR = DIRECTORY_SEPARATOR;

    /**
     * Include path separator.
     */
    const DELIMITER = PATH_SEPARATOR;

    /**
     * Namespace package separator.
     */
    const PACKAGE = '\\';

    /**
     * Parse the file path to remove absolute paths and replace with a constant name.
     *
     * @param string $file
     * @param array $paths
     * @return string
     */
    public static function alias($file, array $paths = array()) {
        if (empty($file)) {
            return '[internal]';
        }

        $file = static::ds($file);

        // Inherit titon constants
        foreach (array('vendor', 'app', 'modules', 'resources', 'temp', 'views', 'web') as $type) {
            $constant = strtoupper($type) . '_DIR';

            if (empty($paths[$type]) && defined($constant)) {
                $paths[$type] = constant($constant);
            }
        }

        // Define source locations
        foreach (array('src', 'lib') as $source) {
            if (empty($paths[$source]) && strpos($file, $source) !== false) {
                $parts = explode($source, $file);
                $paths[$source] = $parts[0] . $source;
            }
        }

        foreach ($paths as $key => $path) {
            $path = trim(static::ds($path), static::SEPARATOR) . static::SEPARATOR;

            if (mb_strpos($file, $path) !== false) {
                $file = trim(str_replace($path, '[' . $key . ']', $file), static::SEPARATOR);
                break;
            }
        }

        return $file;
    }

    /**
     * Strips the namespace to return the base class name.
     *
     * @param string $class
     * @param string $separator
     * @return string
     */
    public static function className($class, $separator = self::PACKAGE) {
        return trim(strrchr($class, $separator), $separator);
    }

    /**
     * Converts OS directory separators to the standard forward slash.
     *
     * @param string $path
     * @param bool $endSlash
     * @return string
     */
    public static function ds($path, $endSlash = false) {
        $path = str_replace(array('\\', '/'), static::SEPARATOR, $path);

        if ($endSlash && substr($path, -1) !== static::SEPARATOR) {
            $path .= static::SEPARATOR;
        }

        return $path;
    }

    /**
     * Return the extension from a file path.
     *
     * @param string $path
     * @return string
     */
    public static function ext($path) {
        return strtolower(pathinfo($path, PATHINFO_EXTENSION));
    }

    /**
     * Define additional include paths for PHP to detect within.
     *
     * @param string|array $paths
     * @return array
     */
    public static function includePath($paths) {
        $current = array(get_include_path());

        if (is_array($paths)) {
            $current = array_merge($current, $paths);
        } else {
            $current[] = $paths;
        }

        $path = implode(static::DELIMITER, $current);

        set_include_path($path);

        return $path;
    }

    /**
     * Verify a path is absolute by checking the first path part.
     *
     * @param string $path
     * @return bool
     */
    public static function isAbsolute($path) {
        return ($path[0] === '/' || $path[0] === '\\' || preg_match('/^[a-zA-Z0-9]+:/', $path));
    }

    /**
     * Verify a path is relative.
     *
     * @param string $path
     * @return bool
     */
    public static function isRelative($path) {
        return !static::isAbsolute($path);
    }

    /**
     * Join all path parts and return a normalized path.
     *
     * @param array $paths
     * @param bool $above - Go above the root path if .. is used
     * @param bool $join - Join all the path parts into a string
     * @return string
     * @throws \Titon\Utility\Exception\InvalidTypeException
     */
    public static function join(array $paths, $above = true, $join = true) {
        $clean = array();
        $parts = array();
        $ds = static::SEPARATOR;
        $up = 0;

        // First pass expands sub-paths
        foreach ($paths as $path) {
            if (!is_string($path)) {
                throw new InvalidTypeException('Path parts must be strings');
            }

            $path = trim(static::ds($path), $ds);

            if (strpos($path, $ds) !== false) {
                $clean = array_merge($clean, explode($ds, $path));
            } else {
                $clean[] = $path;
            }
        }

        // Second pass flattens dot paths
        foreach (array_reverse($clean) as $path) {
            if ($path === '.' || !$path) {
                continue;

            } else if ($path === '..') {
                $up++;

            } else if ($up) {
                $up--;

            } else {
                $parts[] = $path;
            }
        }

        // Append double dots above root
        if ($above) {
            while ($up) {
                $parts[] = '..';
                $up--;
            }
        }

        $parts = array_reverse($parts);

        if ($join) {
            return implode($parts, $ds);
        }

        return $parts;
    }

    /**
     * Normalize a string by resolving "." and "..". When multiple slashes are found, they're replaced by a single one;
     * when the path contains a trailing slash, it is preserved. On Windows backslashes are used.
     *
     * @param string $path
     * @return string
     */
    public static function normalize($path) {
        return realpath($path);
    }

    /**
     * Returns a namespace with only the base package and not the class name.
     *
     * @param string $class
     * @param string $separator
     * @return string
     */
    public static function packageName($class, $separator = self::PACKAGE) {
        return trim(mb_substr($class, 0, mb_strrpos($class, $separator)), $separator);
    }

    /**
     * Determine the relative path between two absolute paths.
     *
     * @param string $from
     * @param string $to
     * @return string
     * @throws \Titon\Utility\Exception\InvalidArgumentException
     */
    public static function relativeTo($from, $to) {
        if (static::isRelative($from) || static::isRelative($to)) {
            throw new InvalidArgumentException('Cannot determine relative path without two absolute paths');
        }

        $ds = static::SEPARATOR;
        $from = explode($ds, static::ds($from, true));
        $to = explode($ds, static::ds($to, true));
        $relative = $to;

        foreach ($from as $depth => $dir) {
            // Find first non-matching dir and ignore it
            if ($dir === $to[$depth]) {
                array_shift($relative);

            // Get count of remaining dirs to $from
            } else {
                $remaining = count($from) - $depth;

                // Add traversals up to first matching dir
                if ($remaining > 1) {
                    $padLength = (count($relative) + $remaining - 1) * -1;
                    $relative = array_pad($relative, $padLength, '..');
                    break;
                } else {
                    $relative[0] = '.' . $ds . $relative[0];
                }
            }
        }

        if (!$relative) {
            return '.' . $ds;
        }

        return implode($ds, $relative);
    }

    /**
     * Strip off the extension if it exists.
     *
     * @param string $path
     * @return string
     */
    public static function stripExt($path) {
        if (mb_strpos($path, '.') !== false) {
            $path = mb_substr($path, 0, mb_strrpos($path, '.'));
        }

        return $path;
    }

    /**
     * Converts a path to a namespace package.
     *
     * @param string $path
     * @return string
     */
    public static function toNamespace($path) {
        $path = static::ds(static::stripExt($path));

        // Attempt to split path at source folder
        foreach (array('lib', 'src') as $folder) {
            if (mb_strpos($path, $folder . static::SEPARATOR) !== false) {
                $paths = explode($folder . static::SEPARATOR, $path);
                $path = $paths[1];
            }
        }

        return trim(str_replace('/', static::PACKAGE, $path), static::PACKAGE);
    }

    /**
     * Converts a namespace to a relative or absolute file system path.
     *
     * @param string $path
     * @param string $ext
     * @param string $root
     * @return string
     */
    public static function toPath($path, $ext = 'php', $root = '') {
        $ds = static::SEPARATOR;
        $path = static::ds($path);
        $dirs = explode($ds, $path);
        $file = array_pop($dirs);
        $path = implode($ds, $dirs) . $ds . str_replace('_', $ds, $file);

        if ($ext && mb_substr($path, -mb_strlen($ext)) !== $ext) {
            $path .= '.' . $ext;
        }

        if ($root) {
            $path = $root . $path;
        }

        return $path;
    }

}