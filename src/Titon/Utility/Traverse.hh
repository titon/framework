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
     * Check if the collection is traversable. If not, throw an exception.
     * This must be done until a traversable type hint is added, or arrays are removed.
     *
     * @param mixed $collection
     * @return bool
     */
    public static function check(mixed $collection): bool {
        if (is_traversable($collection)) {
            return true;
        }

        throw new \InvalidArgumentException('Collection must either be an array or an object that extends Traversable.');
    }

    /**
     * Determines the total depth of a multi-dimensional array or object.
     * Has two methods of determining depth: based on recursive depth, or based on tab indentation (faster).
     *
     * @param mixed $collection
     * @return int
     */
    public static function depth(mixed $collection): int {
        static::check($collection);

        if (empty($collection)) {
            return 0;
        }

        $depth = 1;

        foreach ($collection as $value) {
            if (is_traversable($value)) {
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
     * @param mixed $collection
     * @param callable $callback
     * @param bool $recursive
     * @return Collection
     */
    public static function each(mixed $collection, callable $callback, bool $recursive = true): mixed {
        static::check($collection);

        foreach ($collection as $key => $value) {
            if (is_traversable($value) && $recursive) {
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
     * @param Collection $collection
     * @param callable $callback
     * @return bool
     */
    public static function every(mixed $collection, callable $callback): bool {
        static::check($collection);

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
     * @param Map $collection
     * @param Vector<string> $keys
     * @return mixed
     */
    public static function exclude(mixed $collection, Vector<string> $keys): mixed {
        static::check($collection);

        foreach ($keys as $key) {
            unset($collection[$key]);
        }

        return $collection;
    }

    /**
     * Expand an array to a fully workable multi-dimensional array, where the values key is a dot notated path.
     *
     * @param Map $collection
     * @return Map<string, mixed>
     */
    public static function expand(mixed $collection): Map<string, mixed> {
        static::check($collection);

        $data = Map {};

        foreach ($collection as $key => $value) {
            $data = static::insert($data, $key, $value);
        }

        return $data;
    }

    /**
     * Extract the value of an array, depending on the paths given, represented by key.key.key notation.
     *
     * @param mixed $collection
     * @param string $path
     * @return mixed
     */
    public static function extract(mixed $collection, string $path): ?mixed {
        static::check($collection);

        $data = $collection;

        foreach (explode('.', $path) as $key) {
            if (!is_traversable($data) || !isset($data[$key])) {
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
     * @param mixed $collection
     * @param bool $recursive
     * @param callable $callback
     * @return mixed
     */
    public static function filter(mixed $collection, bool $recursive = true, ?callable $callback = null): mixed {
        static::check($collection);

        if ($callback === null) {
            $callback = function($var) {
                return ($var === 0 || $var === '0' || $var);
            };
        }

        if ($recursive) {
            foreach ($collection as $key => $value) {
                if (is_traversable($value)) {
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
     * @param mixed $collection
     * @param string $path
     * @return Map<string, mixed>
     */
    public static function flatten(mixed $collection, string $path = ''): Map<string, mixed> {
        static::check($collection);

        if ($path) {
            $path .= '.';
        }

        $data = Map {};

        foreach ($collection as $key => $value) {
            if (is_traversable($value)) {
                if (empty($value)) {
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
     * @param mixed $collection
     * @param string $path
     * @return mixed
     */
    public static function get(mixed $collection, ?string $path = ''): ?mixed {
        static::check($collection);

        // Allow whole collection to be returned
        if (!$path) {
            return $collection;
        }

        return static::extract($collection, $path);
    }

    /**
     * Checks to see if a key/value pair exists within an array, determined by the given path.
     *
     * @param mixed $collection
     * @param string $path
     * @return bool
     */
    public static function has(mixed $collection, string $path): bool {
        static::check($collection);

        $data = $collection;

        foreach (explode('.', $path) as $key) {
            if (!is_traversable($data) || !isset($data[$key])) {
                return false;
            }

            $data = $data[$key];
        }

        return true;
    }

    /**
     * Includes the specified key-value pair in the set if the key doesn't already exist.
     *
     * @param mixed $collection
     * @param string $path
     * @param mixed $value
     * @return mixed
     */
    public static function inject(mixed $collection, string $path, mixed $value): mixed {
        static::check($collection);

        if (static::has($collection, $path)) {
            return $collection;
        }

        return static::insert($collection, $path, $value);
    }

    /**
     * Inserts a value into the array set based on the given path.
     *
     * @param mixed $collection
     * @param string $path
     * @param mixed $value
     * @return mixed
     */
    public static function insert(mixed $collection, string $path, mixed $value): mixed {
        static::check($collection);

        $paths = explode('.', $path);

        // We have to use references for arrays
        if (is_array($collection)) {
            $data =& $collection;

            while (count($paths) > 1) {
                $key = array_shift($paths);

                if (!isset($data[$key]) || !is_traversable($data[$key])) {
                    $data[$key] = [];
                }

                $data =& $data[$key];
            }

        } else {
            $data = $collection;

            while (count($paths) > 1) {
                $key = array_shift($paths);

                if (!isset($data[$key]) || !is_traversable($data[$key])) {
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
     * @param mixed $collection
     * @param bool $strict
     * @return bool
     */
    public static function isAlpha(mixed $collection, bool $strict = true): bool {
        static::check($collection);

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
     * @param mixed $collection
     * @return bool
     */
    public static function isNumeric(mixed $collection): bool {
        static::check($collection);

        return static::every($collection, function($value) {
            return is_numeric($value);
        });
    }

    /**
     * Returns the key of the specified value. Will recursively search if the first pass doesn't match.
     *
     * @param mixed $collection
     * @param mixed $match
     * @return mixed
     */
    public static function keyOf(mixed $collection, mixed $match): ?mixed {
        static::check($collection);

        $return = null;
        $isArray = Vector {};

        foreach ($collection as $key => $value) {
            if ($value === $match) {
                $return = $key;
            }

            if (is_traversable($value)) {
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
     * @param mixed $base
     * @param mixed $collections
     * @return mixed
     */
    public static function merge(mixed $base, ...$collections): mixed { // todo - Add type hinting when variadic supports it
        static::check($base);

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

                    if (is_traversable($value) && is_traversable($current)) {
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
     * @param mixed $collection
     * @param string $path
     * @return Vector<mixed>
     */
    public static function pluck(mixed $collection, string $path): Vector<mixed> {
        static::check($collection);

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
     * @param mixed $collection
     * @param Vector<string> $keys
     * @return Map<string, mixed>
     */
    public static function reduce(mixed $collection, Vector<string> $keys): Map<string, mixed> {
        static::check($collection);

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
     * @param mixed $collection
     * @param string $path
     * @return mixed
     */
    public static function remove(mixed $collection, string $path): mixed {
        static::check($collection);

        $paths = explode('.', $path);

        // We have to use references for arrays
        if (is_array($collection)) {
            $data =& $collection;

            while (count($paths) > 1) {
                $key = array_shift($paths);

                if (!isset($data[$key]) || !is_traversable($data[$key])) {
                    return $collection;
                }

                $data =& $data[$key];
            }

        } else {
            $data = $collection;

            while (count($paths) > 1) {
                $key = array_shift($paths);

                if (!isset($data[$key]) || !is_traversable($data[$key])) {
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
     * @param mixed $collection
     * @param Map|string $path
     * @param mixed $value
     * @return mixed
     */
    public static function set(mixed $collection, mixed $path, ?mixed $value = null): mixed {
        static::check($collection);

        if (is_traversable($path)) {
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
     * @param mixed $collection
     * @param \Closure $callback
     * @return bool
     */
    public static function some(mixed $collection, Closure $callback): bool {
        static::check($collection);

        foreach ($collection as $key => $value) {
            if (call_user_func_array($callback, [$value, $key])) {
                return true;
            }
        }

        return false;
    }

}
