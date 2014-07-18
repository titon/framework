<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Utility;

use Titon\Common\Macroable;
use Titon\Utility\Exception\InvalidArgumentException;

/**
 * Provides convenience functions for inflecting notation paths, namespace paths and file system paths.
 *
 * @package Titon\Utility
 */
class Path {
    use Macroable;

    /**
     * Directory separator.
     */
    const string SEPARATOR = DIRECTORY_SEPARATOR;

    /**
     * Include path separator.
     */
    const string DELIMITER = PATH_SEPARATOR;

    /**
     * Namespace package separator.
     */
    const string PACKAGE = '\\';

    /**
     * Parse the file path to remove absolute paths and replace with a constant name.
     *
     * @param string $file
     * @param Map<string, string> $paths
     * @return string
     */
    public static function alias(?string $file, ?Map<string, string> $paths = Map {}): string {
        if (!$file) {
            return '[internal]';
        }

        $file = static::ds($file);

        // Inherit titon constants
        foreach (Vector {'vendor', 'app', 'modules', 'resources', 'temp', 'views', 'web'} as $type) {
            $constant = strtoupper($type) . '_DIR';

            if (empty($paths[$type]) && defined($constant)) {
                $paths[$type] = constant($constant);
            }
        }

        // Define source locations
        foreach (Vector {'src', 'lib'} as $source) {
            if (empty($paths[$source]) && strpos($file, $source) !== false) {
                $parts = explode($source, $file);
                $paths[$source] = $parts[0] . $source;
            }
        }

        foreach ($paths as $key => $path) {
            $path = trim(static::ds($path), '/') . '/';

            if (mb_strpos($file, $path) !== false) {
                $file = trim(str_replace($path, '[' . $key . ']', $file), '/');
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
    public static function className(string $class, string $separator = self::PACKAGE): string {
        return trim(strrchr($class, $separator), $separator);
    }

    /**
     * Converts OS directory separators to the standard forward slash.
     *
     * @param string $path
     * @param bool $endSlash
     * @return string
     */
    public static function ds(string $path, bool $endSlash = false): string {
        $path = str_replace('\\', '/', $path);

        if ($endSlash && substr($path, -1) !== '/') {
            $path .= '/';
        }

        return $path;
    }

    /**
     * Return the extension from a file path.
     *
     * @param string $path
     * @return string
     */
    public static function ext(string $path): string {
        return strtolower(pathinfo($path, PATHINFO_EXTENSION));
    }

    /**
     * Define additional include paths for PHP to detect within.
     *
     * @param Vector<string> $paths
     * @return string
     */
    public static function includePath(Vector<string> $paths): string {
        $current = Vector {get_include_path()};
        $current->addAll($paths);

        $path = implode(self::DELIMITER, $current);

        set_include_path($path);

        return $path;
    }

    /**
     * Verify a path is absolute by checking the first path part.
     *
     * @param string $path
     * @return bool
     */
    public static function isAbsolute(string $path): bool {
        return ($path[0] === '/' || $path[0] === '\\' || preg_match('/^[a-zA-Z0-9]+:/', $path));
    }

    /**
     * Verify a path is relative.
     *
     * @param string $path
     * @return bool
     */
    public static function isRelative(string $path): bool {
        return !static::isAbsolute($path);
    }

    /**
     * Join all path parts and return a normalized path.
     *
     * @param Vector<string> $paths
     * @param bool $above - Go above the root path if .. is used
     * @param bool $join - Join all the path parts into a string
     * @return string|Vector<string>
     */
    public static function join(Vector<string> $paths, bool $above = true, bool $join = true): mixed {
        $clean = Vector {};
        $parts = Vector {};
        $up = 0;

        // First pass expands sub-paths
        foreach ($paths as $path) {
            $path = trim(static::ds($path), '/');

            if (strpos($path, '/') !== false) {
                $clean->addAll(new Vector(explode('/', $path)));
            } else {
                $clean[] = $path;
            }
        }

        // Second pass flattens dot paths
        $clean->reverse();

        foreach ($clean as $path) {
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

        $parts->reverse();

        if ($join) {
            return implode($parts, '/');
        }

        return $parts;
    }

    /**
     * Normalize a string by resolving "." and "..". When multiple slashes are found, they're replaced by a single one;
     * when the path contains a trailing slash, it is preserved. On Windows backslashes are used.
     *
     * @param string $path
     * @return string
     * @codeCoverageIgnore
     */
    public static function normalize(string $path): string {
        return realpath($path);
    }

    /**
     * Returns a namespace with only the base package and not the class name.
     *
     * @param string $class
     * @param string $separator
     * @return string
     */
    public static function packageName(string $class, string $separator = self::PACKAGE): string {
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
    public static function relativeTo(string $from, string $to): string {
        if (static::isRelative($from) || static::isRelative($to)) {
            throw new InvalidArgumentException('Cannot determine relative path without two absolute paths');
        }

        $from = explode('/', static::ds($from, true));
        $to = explode('/', static::ds($to, true));
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
                    $relative[0] = './' . $relative[0];
                }
            }
        }

        if (!$relative) {
            return './';
        }

        return implode('/', $relative);
    }

    /**
     * Strip off the extension if it exists.
     *
     * @param string $path
     * @return string
     */
    public static function stripExt(string $path): string {
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
    public static function toNamespace(string $path): string {
        $path = static::ds(static::stripExt($path));

        // Attempt to split path at source folder
        foreach (Vector {'lib', 'src'} as $folder) {
            if (mb_strpos($path, $folder . '/') !== false) {
                $paths = explode($folder . '/', $path);
                $path = $paths[1];
            }
        }

        return trim(str_replace('/', self::PACKAGE, $path), self::PACKAGE);
    }

    /**
     * Converts a namespace to a relative or absolute file system path.
     *
     * @param string $path
     * @param string $ext
     * @param string $root
     * @return string
     */
    public static function toPath(string $path, string $ext = 'php', string $root = ''): string {
        $path = static::ds($path);
        $dirs = explode('/', $path);
        $file = array_pop($dirs);
        $path = implode('/', $dirs) . '/' . str_replace('_', '/', $file);

        if ($ext && mb_substr($path, -mb_strlen($ext)) !== $ext) {
            $path .= '.' . $ext;
        }

        if ($root) {
            $path = $root . $path;
        }

        return $path;
    }

}