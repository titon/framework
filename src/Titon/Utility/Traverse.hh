<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Utility;

use Titon\Common\Macroable;
use \Closure;

/**
 * Mutates and traverses multiple types of data structures (collections).
 *
 * @package Titon\Utility
 */
class Traverse {
    use Macroable;

    /**
     * Determines the total depth of a multi-dimensional array or object.
     * Has two methods of determining depth: based on recursive depth, or based on tab indentation (faster).
     *
     * @param Traversable $collection
     * @return int
     */
    public static function depth(Traversable $collection): int {
        if (count($collection) === 0) {
            return 0;
        }

        $depth = 1;

        foreach ($collection as $value) {
            if ($value instanceof Traversable) {
                $count = static::depth($value) + 1;

                if ($count > $depth) {
                    $depth = $count;
                }
            }
        }

        return $depth;
    }

    /**
     * Calls a function for each key-value pair in the set.
     * If recursive is true, will apply the callback to nested arrays as well.
     *
     * @param Traversable $collection
     * @param callable $callback
     * @param bool $recursive
     * @return Collection
     */
    public static function each(Traversable $collection, callable $callback, bool $recursive = true): mixed {
        foreach ($collection as $key => $value) {
            if ($value instanceof Traversable && $recursive) {
                $collection[$key] = static::each($value, $callback, $recursive);
            } else {
                $collection[$key] = call_user_func_array($callback, [$value, $key]);
            }
        }

        return $collection;
    }

    /**
     * Returns true if every element in the array satisfies the provided testing function.
     *
     * @param Traversable $collection
     * @param callable $callback
     * @return bool
     */
    public static function every(Traversable $collection, callable $callback): bool {
        foreach ($collection as $key => $value) {
            if (!call_user_func_array($callback, [$value, $key])) {
                return false;
            }
        }

        return true;
    }

    /**
     * Exclude specific keys from the array and return the new array.
     *
     * @param Traversable $collection
     * @param Vector<string> $keys
     * @return mixed
     */
    public static function exclude(Traversable $collection, Vector<string> $keys): mixed {
        foreach ($keys as $key) {
            unset($collection[$key]);
        }

        return $collection;
    }

    /**
     * Expand an array to a fully workable multi-dimensional array, where the values key is a dot notated path.
     *
     * @param Traversable $collection
     * @return Map<string, mixed>
     */
    public static function expand(Traversable $collection): Map<string, mixed> {
        $data = Map {};

        foreach ($collection as $key => $value) {
            $data = static::insert($data, $key, $value);
        }

        return $data;
    }

    /**
     * Extract the value of an array, depending on the paths given, represented by key.key.key notation.
     *
     * @param Traversable $collection
     * @param string $path
     * @return mixed
     */
    public static function extract(Traversable $collection, string $path): mixed {
        $data = $collection;

        foreach (explode('.', $path) as $key) {
            if (!$data instanceof Traversable || !isset($data[$key])) {
                return null;
            }

            $data = $data[$key];
        }

        return $data;
    }

    /**
     * Filter out all keys within an array that have an empty value, excluding 0 (string and numeric).
     * If $recursive is set to true, will remove all empty values within all sub-arrays.
     *
     * @param Traversable $collection
     * @param bool $recursive
     * @param callable $callback
     * @return mixed
     */
    public static function filter(Traversable $collection, bool $recursive = true, ?callable $callback = null): mixed {
        if ($callback === null) {
            $callback = function($var) {
                return ($var === 0 || $var === '0' || $var);
            };
        }

        if ($recursive) {
            foreach ($collection as $key => $value) {
                if ($value instanceof Traversable) {
                    $collection[$key] = static::filter($value, $recursive, $callback);
                }
            }
        }

        if ($collection instanceof Collection) {
            return $collection->filter($callback);
        }

        return array_filter($collection, $callback);
    }

    /**
     * Flatten a multi-dimensional array by returning the values with their keys representing their previous pathing.
     *
     * @param Traversable $collection
     * @param string $path
     * @return Map<string, mixed>
     */
    public static function flatten(Traversable $collection, string $path = ''): Map<string, mixed> {
        if ($path) {
            $path .= '.';
        }

        $data = Map {};

        foreach ($collection as $key => $value) {
            if ($value instanceof Traversable) {
                if (!$value) {
                    $data[$path . $key] = null;
                } else {
                    $data = static::merge($data, static::flatten($value, $path . $key));
                }
            } else {
                $data[$path . $key] = $value;
            }
        }

        return $data;
    }

    /**
     * Get a value from the set. If they path doesn't exist, return null, or if the path is empty, return the whole set.
     *
     * @param Traversable $collection
     * @param string $path
     * @return mixed
     */
    public static function get(Traversable $collection, string $path = ''): ?mixed {
        if (!$path) {
            return $collection; // Allow whole collection to be returned
        }

        return static::extract($collection, $path);
    }

    /**
     * Checks to see if a key/value pair exists within an array, determined by the given path.
     *
     * @param Traversable $collection
     * @param string $path
     * @return bool
     */
    public static function has(Traversable $collection, string $path): bool {
        $data = $collection;

        foreach (explode('.', $path) as $key) {
            if (!$data instanceof Traversable || !isset($data[$key])) {
                return false;
            }

            $data = $data[$key];
        }

        return true;
    }

    /**
     * Includes the specified key-value pair in the set if the key doesn't already exist.
     *
     * @param Traversable $collection
     * @param string $path
     * @param mixed $value
     * @return mixed
     */
    public static function inject(Traversable $collection, string $path, mixed $value): mixed {
        if (static::has($collection, $path)) {
            return $collection;
        }

        return static::insert($collection, $path, $value);
    }

    /**
     * Inserts a value into the array set based on the given path.
     *
     * @param Traversable $collection
     * @param string $path
     * @param mixed $value
     * @return mixed
     */
    public static function insert(Traversable $collection, string $path, mixed $value): mixed {
        $paths = explode('.', $path);

        // We have to use references for arrays
        if (is_array($collection)) {
            $data =& $collection;

            while (count($paths) > 1) {
                $key = array_shift($paths);

                if (!isset($data[$key]) || !$data[$key] instanceof Traversable) {
                    $data[$key] = [];
                }

                $data =& $data[$key];
            }

        } else {
            $data = $collection;

            while (count($paths) > 1) {
                $key = array_shift($paths);

                if (!isset($data[$key]) || !$data[$key] instanceof Traversable) {
                    $data[$key] = Map {};
                }

                $data = $data[$key];
            }
        }

        // Set the last path value
        $data[$paths[0]] = $value;

        return $collection;
    }

    /**
     * Checks to see if all values in the array are strings, returns false if not.
     * If $strict is true, method will fail if there are values that are numerical strings, but are not cast as integers.
     *
     * @param Traversable $collection
     * @param bool $strict
     * @return bool
     */
    public static function isAlpha(Traversable $collection, bool $strict = true): bool {
        return static::every($collection, function($value) use ($strict) {
            if (!is_string($value)) {
                return false;
            }

            if ($strict) {
                if (is_string($value) && is_numeric($value)) {
                    return false;
                }
            }

            return true;
        });
    }

    /**
     * Checks to see if all values in the array are numeric, returns false if not.
     *
     * @param Traversable $collection
     * @return bool
     */
    public static function isNumeric(Traversable $collection): bool {
        return static::every($collection, function($value) {
            return is_numeric($value);
        });
    }

    /**
     * Returns the key of the specified value. Will recursively search if the first pass doesn't match.
     *
     * @param Traversable $collection
     * @param mixed $match
     * @return mixed
     */
    public static function keyOf(Traversable $collection, mixed $match): mixed {
        $return = null;
        $isArray = Vector {};

        foreach ($collection as $key => $value) {
            if ($value === $match) {
                $return = $key;
            }

            if ($value instanceof Traversable) {
                $isArray[] = $key;
            }
        }

        if (!$return && $isArray) {
            foreach ($isArray as $key) {
                if ($value = static::keyOf($collection[$key], $match)) {
                    $return = $key . '.' . $value;
                }
            }
        }

        return $return;
    }

    /**
     * Merge is a combination of array_merge() and array_merge_recursive(). However, when merging two keys with the same key,
     * the previous value will be overwritten instead of being added into an array. The later array takes precedence when merging.
     *
     * @param Traversable $base
     * @param Traversable $collections
     * @return mixed
     */
    public static function merge(Traversable $base, ...$collections): mixed { // todo - Add type hinting when variadic supports it
        $isVector = false;

        // Clone the collection so we don't modify the base reference
        if ($base instanceof Map) {
            $data = $base->toMap();

        } else if ($base instanceof Vector) {
            $data = $base->toVector();
            $isVector = true;

        } else {
            $data = $base;
        }

        foreach ($collections as $collection) {
            foreach ($collection as $key => $value) {
                if (isset($data[$key])) {
                    $current = $data[$key];

                    if ($value instanceof Traversable && $current instanceof Traversable) {
                        $value = static::merge($current, $value);
                    }
                }

                if ($isVector) {
                    $data[] = $value;
                } else {
                    $data[$key] = $value;
                }
            }
        }

        return $data;
    }

    /**
     * Pluck a value out of each child-array and return an array of the plucked values.
     *
     * @param Traversable $collection
     * @param string $path
     * @return Vector<mixed>
     */
    public static function pluck(Traversable $collection, string $path): Vector<mixed> {
        $data = Vector {};

        foreach ($collection as $coll) {
            if ($value = static::extract($coll, $path)) {
                $data[] = $value;
            }
        }

        return $data;
    }

    /**
     * Reduce an array by removing all keys that have not been defined for persistence.
     *
     * @param Traversable $collection
     * @param Vector<string> $keys
     * @return Map<string, mixed>
     */
    public static function reduce(Traversable $collection, Vector<string> $keys): Map<string, mixed> {
        $map = Map {};

        foreach ($collection as $key => $value) {
            if (in_array($key, $keys)) {
                $map[$key] = $value;
            }
        }

        return $map;
    }

    /**
     * Remove an index from the array, determined by the given path.
     *
     * @param Traversable $collection
     * @param string $path
     * @return mixed
     */
    public static function remove(Traversable $collection, string $path): mixed {
        $paths = explode('.', $path);

        // We have to use references for arrays
        if (is_array($collection)) {
            $data =& $collection;

            while (count($paths) > 1) {
                $key = array_shift($paths);

                if (!isset($data[$key]) || !$data[$key] instanceof Traversable) {
                    return $collection;
                }

                $data =& $data[$key];
            }

        } else {
            $data = $collection;

            while (count($paths) > 1) {
                $key = array_shift($paths);

                if (!isset($data[$key]) || !$data[$key] instanceof Traversable) {
                    return $collection;
                }

                $data = $data[$key];
            }
        }

        // Remove the last path value
        unset($data[$paths[0]]);

        return $collection;
    }

    /**
     * Set a value into the result set. If the paths is an array, loop over each one and insert the value.
     *
     * @param Traversable $collection
     * @param Map|string $path
     * @param mixed $value
     * @return mixed
     */
    public static function set(Traversable $collection, mixed $path, ?mixed $value = null): mixed {
        if ($path instanceof Traversable) {
            foreach ($path as $key => $value) {
                $collection = static::insert($collection, $key, $value);
            }
        } else {
            $collection = static::insert($collection, $path, $value);
        }

        return $collection;
    }

    /**
     * Returns true if at least one element in the array satisfies the provided testing function.
     *
     * @param Traversable $collection
     * @param \Closure $callback
     * @return bool
     */
    public static function some(Traversable $collection, Closure $callback): bool {
        foreach ($collection as $key => $value) {
            if (call_user_func_array($callback, [$value, $key])) {
                return true;
            }
        }

        return false;
    }

}
