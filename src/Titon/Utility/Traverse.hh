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
     * @param Collection $collection
     * @return int
     */
    public static function depth(Collection $collection): int {
        if ($collection->isEmpty()) {
            return 0;
        }

        $depth = 1;

        foreach ($collection as $value) {
            if ($value instanceof Collection) {
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
     * @param Collection $collection
     * @param \Closure $callback
     * @param bool $recursive
     * @return Collection
     */
    public static function each(Collection $collection, Closure $callback, bool $recursive = true): Collection {
        foreach ($collection as $key => $value) {
            if ($value instanceof Collection && $recursive) {
                $collection->set($key, static::each($value, $callback, $recursive));
            } else {
                $collection->set($key, call_user_func_array($callback, [$value, $key]));
            }
        }

        return $collection;
    }

    /**
     * Returns true if every element in the array satisfies the provided testing function.
     *
     * @param Collection $collection
     * @param \Closure $callback
     * @return bool
     */
    public static function every(Collection $collection, Closure $callback): bool {
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
     * @return Map<string, mixed>
     */
    public static function exclude(Map $collection, Vector<string> $keys): Map<string, mixed> {
        foreach ($keys as $key) {
            $collection->remove($key);
        }

        return $collection;
    }

    /**
     * Expand an array to a fully workable multi-dimensional array, where the values key is a dot notated path.
     *
     * @param Map $collection
     * @return Map<string, mixed>
     */
    public static function expand(Map $collection): Map<string, mixed> {
        $data = Map {};

        foreach ($collection as $key => $value) {
            $data = static::insert($data, $key, $value);
        }

        return $data;
    }

    /**
     * Extract the value of an array, depending on the paths given, represented by key.key.key notation.
     *
     * @param Map $collection
     * @param string $path
     * @return mixed
     */
    public static function extract(Map $collection, string $path): ?mixed {

        // Exit early for faster processing
        if (strpos($path, '.') === false) {
            return $collection->get($path);
        }

        $search = $collection;
        $paths = explode('.', $path);
        $total = count($paths);

        while ($total > 0) {
            $key = $paths[0];
            $value = $search->get($key);

            // Within the last path
            if ($total === 1) {
                return $value;

            // Break out of non-existent paths early
            } else if (!$search->contains($key) || !($value instanceof Collection)) {
                break;
            }

            $search = $value;
            array_shift($paths);
            $total--;
        }

        return null;
    }

    /**
     * Filter out all keys within an array that have an empty value, excluding 0 (string and numeric).
     * If $recursive is set to true, will remove all empty values within all sub-arrays.
     *
     * @param Collection $collection
     * @param bool $recursive
     * @param \Closure $callback
     * @return Collection
     */
    public static function filter(Collection $collection, bool $recursive = true, ?Closure $callback = null): Collection {
        if ($callback === null) {
            $callback = function($var) {
                return ($var === 0 || $var === '0' || !empty($var));
            };
        }

        if ($recursive) {
            foreach ($collection as $key => $value) {
                if ($value instanceof Collection) {
                    $collection->set($key, static::filter($value, $recursive, $callback));
                }
            }
        }

        return $collection->filter($callback);
    }

    /**
     * Flatten a multi-dimensional array by returning the values with their keys representing their previous pathing.
     *
     * @param Collection $collection
     * @param string $path
     * @return Map<string, mixed>
     */
    public static function flatten(Collection $collection, string $path = ''): Map<string, mixed> {
        if ($path) {
            $path = $path . '.';
        }

        $data = Map {};

        foreach ($collection as $key => $value) {
            if ($value instanceof Collection) {
                if ($value->isEmpty()) {
                    $data->set($path . $key, null);
                } else {
                    $data->setAll(static::flatten($value, $path . $key));
                }
            } else {
                $data->set($path . $key, $value);
            }
        }

        return $data;
    }

    /**
     * Get a value from the set. If they path doesn't exist, return null, or if the path is empty, return the whole set.
     *
     * @param Map $collection
     * @param string $path
     * @return mixed
     */
    public static function get(Map $collection, ?string $path = ''): ?mixed {

        // Allow whole collection to be returned
        if (!$path) {
            return $collection;
        }

        return static::extract($collection, $path);
    }

    /**
     * Checks to see if a key/value pair exists within an array, determined by the given path.
     *
     * @param Map $collection
     * @param string $path
     * @return bool
     */
    public static function has(Map $collection, string $path): bool {
        if ($collection->isEmpty() || !$path) {
            return false;
        }

        // Exit early for faster processing
        if (strpos($path, '.') === false) {
            return $collection->contains($path);
        }

        $search = $collection;
        $paths = explode('.', $path);
        $total = count($paths);

        while ($total > 0) {
            $key = $paths[0];

            // Within the last path
            if ($total === 1) {
                return $search->contains($key);

            // Break out of non-existent paths early
            } else if (!$search->contains($key) || !($search->get($key) instanceof Collection)) {
                break;
            }

            $search = $search->get($key);
            array_shift($paths);
            $total--;
        }

        return false;
    }

    /**
     * Includes the specified key-value pair in the set if the key doesn't already exist.
     *
     * @param Map $collection
     * @param string $path
     * @param mixed $value
     * @return Map<string, mixed>
     */
    public static function inject(Map $collection, string $path, mixed $value): Map<string, mixed> {
        if (static::has($collection, $path)) {
            return $collection;
        }

        return static::insert($collection, $path, $value);
    }

    /**
     * Inserts a value into the array set based on the given path.
     *
     * @param Map $collection
     * @param string $path
     * @param mixed $value
     * @return Map<string, mixed>
     */
    public static function insert(Map $collection, string $path, mixed $value): Map<string, mixed> {
        if (!$path) {
            return $collection;
        }

        // Exit early for faster processing
        if (strpos($path, '.') === false) {
            $collection->set($path, $value);

            return $collection;
        }

        $search = $collection;
        $paths = explode('.', $path);
        $total = count($paths);

        while ($total > 0) {
            $key = $paths[0];

            // Within the last path
            if ($total === 1) {
                $search->set($key, $value);

            // Break out of non-existent paths early
            } else if (!$search->contains($key) || !($search->get($key) instanceof Collection)) {
                $search->set($key, Map {});
            }

            $search = $search->get($key);
            array_shift($paths);
            $total--;
        }

        return $collection;
    }

    /**
     * Checks to see if all values in the array are strings, returns false if not.
     * If $strict is true, method will fail if there are values that are numerical strings, but are not cast as integers.
     *
     * @param Collection $collection
     * @param bool $strict
     * @return bool
     */
    public static function isAlpha(Collection $collection, bool $strict = true): bool {
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
     * @param Collection $collection
     * @return bool
     */
    public static function isNumeric(Collection $collection): bool {
        return static::every($collection, function($value) {
            return is_numeric($value);
        });
    }

    /**
     * Returns the key of the specified value. Will recursively search if the first pass doesn't match.
     *
     * @param Collection $collection
     * @param mixed $match
     * @return mixed
     */
    public static function keyOf(Collection $collection, mixed $match): ?mixed {
        $return = null;
        $isArray = Vector {};

        foreach ($collection as $key => $value) {
            if ($value === $match) {
                $return = $key;
            }

            if (is_array($value) || $value instanceof Collection) {
                $isArray[] = $key;
            }
        }

        if (!$return && $isArray) {
            foreach ($isArray as $key) {
                if ($value = static::keyOf($collection->get($key), $match)) {
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
     * @return Map<string, mixed>
     */
    public static function merge() {
        $collections = func_get_args();
        $data = Map {};

        if (!$collections) {
            return $data;
        }

        foreach ($collections as $collection) {
            foreach ($collection as $key => $value) {
                if ($data->contains($key)) {
                    $current = $data->get($key);

                    if ($value instanceof Collection && $current instanceof Collection) {
                        $value = static::merge($current, $value);
                    }
                }

                $data->set($key, $value);
            }
        }

        return $data;
    }

    /**
     * Pluck a value out of each child-array and return an array of the plucked values.
     *
     * @param Collection $collection
     * @param string $path
     * @return Vector<mixed>
     */
    public static function pluck(Collection $collection, string $path): Vector<mixed> {
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
     * @param Map $collection
     * @param Vector<string> $keys
     * @return Map<string, mixed>
     */
    public static function reduce(Map $collection, Vector<string> $keys): Map<string, mixed> {
        $map = Map {};

        foreach ($collection as $key => $value) {
            if ($keys->linearSearch($key) >= 0) {
                $map->set($key, $value);
            }
        }

        return $map;
    }

    /**
     * Remove an index from the array, determined by the given path.
     *
     * @param Map $collection
     * @param string $path
     * @return Map<string, mixed>
     */
    public static function remove(Map $collection, string $path): Map<string, mixed> {
        if (!$path) {
            return $collection;
        }

        // Exit early for faster processing
        if (strpos($path, '.') === false) {
            $collection->remove($path);

            return $collection;
        }

        $search = $collection;
        $paths = explode('.', $path);
        $total = count($paths);

        while ($total > 0) {
            $key = $paths[0];

            // Within the last path
            if ($total === 1) {
                $search->remove($key);
                break;

            // Break out of non-existent paths early
            } else if (!$search->contains($key) || !($search->get($key) instanceof Collection)) {
                break;
            }

            $search = $search->get($key);
            array_shift($paths);
            $total--;
        }

        return $collection;
    }

    /**
     * Set a value into the result set. If the paths is an array, loop over each one and insert the value.
     *
     * @param Map $collection
     * @param Map|string $path
     * @param mixed $value
     * @return Map<string, mixed>
     */
    public static function set(Collection $collection, mixed $path, ?mixed $value = null): Map<string, mixed> {
        if ($path instanceof Map) {
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
     * @param Collection $collection
     * @param \Closure $callback
     * @return bool
     */
    public static function some(Collection $collection, Closure $callback): bool {
        foreach ($collection as $key => $value) {
            if (call_user_func_array($callback, [$value, $key])) {
                return true;
            }
        }

        return false;
    }

}
