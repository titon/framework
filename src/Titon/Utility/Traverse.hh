<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Utility;

use Titon\Common\Macroable;
use \Indexish;

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
     * @param Indexish<Tk, Tv> $collection
     * @return int
     */
    public static function depth<Tk, Tv>(Indexish<Tk, Tv> $collection): int {
        if (count($collection) === 0) {
            return 0;
        }

        $depth = 1;

        foreach ($collection as $value) {
            if ($value instanceof Indexish) {
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
     * @param Indexish<Tk, mixed> $collection
     * @param (function(Tk, mixed): mixed) $callback
     * @param bool $recursive
     * @return Indexish<Tk, mixed>
     */
    public static function each<Tk>(Indexish<Tk, mixed> $collection, (function(Tk, mixed): mixed) $callback, bool $recursive = true): Indexish<Tk, mixed> {
        foreach ($collection as $key => $value) {
            if ($value instanceof Indexish && $recursive) {
                $collection[$key] = static::each($value, $callback, $recursive);
            } else {
                $collection[$key] = call_user_func_array($callback, [$key, $value]);
            }
        }

        return $collection;
    }

    /**
     * Returns true if every element in the array satisfies the provided testing function.
     *
     * @param Indexish<Tk, Tv> $collection
     * @param (function(Tk, Tv): bool) $callback
     * @return bool
     */
    public static function every<Tk, Tv>(Indexish<Tk, Tv> $collection, (function(Tk, Tv): bool) $callback): bool {
        foreach ($collection as $key => $value) {
            if (!call_user_func_array($callback, [$key, $value])) {
                return false;
            }
        }

        return true;
    }

    /**
     * Exclude specific keys from the array and return the new array.
     *
     * @param Indexish<Tk, Tv> $collection
     * @param Vector<Tk> $keys
     * @return Indexish<Tk, Tv>
     */
    public static function exclude<Tk, Tv>(Indexish<Tk, Tv> $collection, Vector<Tk> $keys): Indexish<Tk, Tv> {
        foreach ($keys as $key) {
            unset($collection[$key]);
        }

        return $collection;
    }

    /**
     * Expand an array to a fully workable multi-dimensional array, where the values key is a dot notated path.
     *
     * @param Indexish<Tk, Tv> $collection
     * @return Indexish<string, mixed>
     */
    public static function expand<Tk, Tv>(Indexish<Tk, Tv> $collection): Indexish<string, mixed> {
        $data = Map {};

        foreach ($collection as $key => $value) {
            $data = static::insert($data, (string) $key, $value);
        }

        return $data;
    }

    /**
     * Extract the value of an array, depending on the paths given, represented by key.key.key notation.
     *
     * @param Indexish<Tk, Tv> $collection
     * @param string $path
     * @return mixed
     */
    public static function extract<Tk, Tv>(Indexish<Tk, Tv> $collection, string $path): mixed {
        $data = $collection;

        foreach (explode('.', $path) as $key) {
            if (!$data instanceof Indexish || !isset($data[$key])) {
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
     * @param Indexish<Tk, mixed> $collection
     * @param bool $recursive
     * @param ?(function(Tk, mixed): bool) $callback
     * @return Indexish<Tk, mixed>
     */
    public static function filter<Tk>(Indexish<Tk, mixed> $collection, bool $recursive = true, ?(function(Tk, mixed): bool) $callback = null): Indexish<Tk, mixed> {
        if ($callback === null) {
            $callback = function($key, $value): bool {
                return ($value === 0 || $value === '0' || $value);
            };
        }

        if ($recursive) {
            foreach ($collection as $key => $value) {
                if ($value instanceof Indexish) {
                    $collection[$key] = static::filter($value, $recursive, $callback);
                }
            }
        }

        if ($collection instanceof MutableMap || $collection instanceof MutableVector) {
            return $collection->filterWithKey($callback);
        }

        return $collection;
    }

    /**
     * Flatten a multi-dimensional array by returning the values with their keys representing their previous pathing.
     *
     * @param Indexish<Tk, Tv> $collection
     * @param string $path
     * @return Indexish<mixed, mixed>
     */
    public static function flatten<Tk, Tv>(Indexish<Tk, Tv> $collection, string $path = ''): Indexish<mixed, mixed> {
        if ($path) {
            $path .= '.';
        }

        $data = Map {};

        foreach ($collection as $key => $value) {
            $key = (string) $key;

            if ($value instanceof Indexish) {
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
     * @param Indexish<Tk, Tv> $collection
     * @param string $path
     * @return mixed
     */
    public static function get<Tk, Tv>(Indexish<Tk, Tv> $collection, string $path = ''): mixed {
        if (!$path) {
            return $collection; // Allow whole collection to be returned
        }

        return static::extract($collection, $path);
    }

    /**
     * Checks to see if a key/value pair exists within an array, determined by the given path.
     *
     * @param Indexish<Tk, Tv> $collection
     * @param string $path
     * @return bool
     */
    public static function has<Tk, Tv>(Indexish<Tk, Tv> $collection, string $path): bool {
        $data = $collection;

        foreach (explode('.', $path) as $key) {
            if (!$data instanceof Indexish || !isset($data[$key])) {
                return false;
            }

            $data = $data[$key];
        }

        return true;
    }

    /**
     * Includes the specified key-value pair in the set if the key doesn't already exist.
     *
     * @param Indexish<Tk, mixed> $collection
     * @param string $path
     * @param mixed $value
     * @return Indexish<Tk, mixed>
     */
    public static function inject<Tk>(Indexish<Tk, mixed> $collection, string $path, mixed $value): Indexish<Tk, mixed> {
        if (static::has($collection, $path)) {
            return $collection;
        }

        return static::insert($collection, $path, $value);
    }

    /**
     * Inserts a value into the array set based on the given path.
     *
     * @param Indexish<Tk, mixed> $collection
     * @param string $path
     * @param mixed $value
     * @return Indexish<Tk, mixed>
     */
    public static function insert<Tk>(Indexish<Tk, mixed> $collection, string $path, mixed $value): Indexish<Tk, mixed> {
        $paths = explode('.', $path);
        $data = $collection;

        while (count($paths) > 1) {
            $key = array_shift($paths);

            if ($data instanceof Indexish && (!isset($data[$key]) || !$data[$key] instanceof Indexish)) {
                $data[$key] = Map {};
            }

            $data = $data[$key];
        }

        // Set the last path value
        $data[$paths[0]] = $value;

        return $collection;
    }

    /**
     * Checks to see if all values in the array are strings, returns false if not.
     * If $strict is true, method will fail if there are values that are numerical strings, but are not cast as integers.
     *
     * @param Indexish<Tk, Tv> $collection
     * @param bool $strict
     * @return bool
     */
    public static function isAlpha<Tk, Tv>(Indexish<Tk, Tv> $collection, bool $strict = true): bool {
        return static::every($collection, function($key, $value): bool use ($strict) {
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
     * @param Indexish<Tk, Tv> $collection
     * @return bool
     */
    public static function isNumeric<Tk, Tv>(Indexish<Tk, Tv> $collection): bool {
        return static::every($collection, function($key, $value): bool {
            return is_numeric($value);
        });
    }

    /**
     * Returns the key of the specified value. Will recursively search if the first pass doesn't match.
     *
     * @param Indexish<Tk, Tv> $collection
     * @param mixed $match
     * @return string
     */
    public static function keyOf<Tk, Tv>(Indexish<Tk, Tv> $collection, mixed $match): string {
        $return = '';

        foreach ($collection as $key => $value) {
            if ($value === $match) {
                $return .= (string) $key;
            }

            if ($value instanceof Indexish) {
                if ($nested = static::keyOf($value, $match)) {
                    $return .= (string) $nested;
                }
            }
        }

        return $return;
    }

    /**
     * Merge is a combination of array_merge() and array_merge_recursive(). However, when merging two keys with the same key,
     * the previous value will be overwritten instead of being added into an array. The later array takes precedence when merging.
     *
     * @param Indexish<mixed, mixed> $base
     * @param Indexish<mixed, mixed> $merge
     * @return Indexish<mixed, mixed>
     */
    /*public static function merge(Indexish<mixed, mixed> $base, Indexish<mixed, mixed> $merge): Indexish<mixed, mixed> {
        foreach ($merge as $key => $value) {
            if (isset($base[$key])) {
                $current = $base[$key];

                if ($value instanceof Indexish && $current instanceof Indexish) {
                    $value = static::merge($current, $value);
                }
            }

            if ($base instanceof Vector) {
                $base[] = $value;
            } else {
                $base[$key] = $value;
            }
        }

        return $base;
    }*/

    /**
     * Pluck a value out of each child-array and return an array of the plucked values.
     *
     * @param Indexish<Tk, Tv> $collection
     * @param string $path
     * @return Vector<mixed>
     */
    public static function pluck<Tk, Tv>(Indexish<Tk, Tv> $collection, string $path): Vector<mixed> {
        $data = Vector {};

        foreach ($collection as $coll) {
            if ($coll instanceof Indexish) {
                if ($value = static::extract($coll, $path)) {
                    $data[] = $value;
                }
            }
        }

        return $data;
    }

    /**
     * Reduce an array by removing all keys that have not been defined for persistence.
     *
     * @param Indexish<Tk, Tv> $collection
     * @param Vector<Tk> $keys
     * @return Indexish<Tk, Tv>
     */
    public static function reduce<Tk, Tv>(Indexish<Tk, Tv> $collection, Vector<Tk> $keys): Indexish<Tk, Tv> {
        $remove = Vector {};

        foreach ($collection as $key => $value) {
            if (!in_array($key, $keys)) {
                $remove[] = $key;
            }
        }

        foreach ($remove as $key) {
            unset($collection[$key]);
        }

        return $collection;
    }

    /**
     * Remove an index from the array, determined by the given path.
     *
     * @param Indexish<Tk, Tv> $collection
     * @param string $path
     * @return Indexish<Tk, Tv>
     */
    public static function remove<Tk, Tv>(Indexish<Tk, Tv> $collection, string $path): Indexish<Tk, Tv> {
        $paths = explode('.', $path);
        $data = $collection;

        while (count($paths) > 1) {
            $key = array_shift($paths);

            if (!$data instanceof Indexish || !isset($data[$key]) || !$data[$key] instanceof Indexish) {
                return $collection;
            }

            $data = $data[$key];
        }

        // Remove the last path value
        if ($data instanceof Indexish) {
            unset($data[$paths[0]]);
        }

        return $collection;
    }

    /**
     * Set a value into the result set. If the paths is an array, loop over each one and insert the value.
     *
     * @param Indexish<Tk, Tv> $collection
     * @param KeyedTraversable|string $path
     * @param mixed $value
     * @return Indexish<Tk, Tv>
     */
    public static function set<Tk, Tv>(Indexish<Tk, Tv> $collection, mixed $path, mixed $value = null): Indexish<Tk, mixed> {
        if ($path instanceof KeyedTraversable) {
            foreach ($path as $key => $value) {
                $collection = static::insert($collection, $key, $value);
            }
        } else {
            $collection = static::insert($collection, (string) $path, $value);
        }

        return $collection;
    }

    /**
     * Returns true if at least one element in the array satisfies the provided testing function.
     *
     * @param Indexish<Tk, Tv> $collection
     * @param (function(Tv, Tk): bool) $callback
     * @return bool
     */
    public static function some<Tk, Tv>(Indexish<Tk, Tv> $collection, (function(Tk, Tv): bool) $callback): bool {
        foreach ($collection as $key => $value) {
            if (call_user_func_array($callback, [$key, $value])) {
                return true;
            }
        }

        return false;
    }

}
