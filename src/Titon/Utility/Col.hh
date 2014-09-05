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
 * Provides accessor, mutator, helper, and traversal capabilities for collections.
 *
 * @package Titon\Utility
 */
class Col {
    use Macroable;

    /**
     * Determines the total depth of a collection.
     *
     * @param Traversable<Tv> $collection
     * @return int
     */
    public static function depth<Tv>(Traversable<Tv> $collection): int {
        if (count($collection) === 0) {
            return 0;
        }

        $depth = 1;

        foreach ($collection as $item) {
            if ($item instanceof Traversable) {
                $count = static::depth($item) + 1;

                if ($count > $depth) {
                    $depth = $count;
                }
            }
        }

        return $depth;
    }

    /**
     * Execute a function for each key-value pair in the collection.
     * If recursive is true, will apply the callback to nested collections as well.
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
     * Returns true if every element in the collection satisfies the provided testing function.
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
     * Exclude specific keys from the map.
     *
     * @param Map<Tk, Tv> $map
     * @param Vector<Tk> $keys
     * @return Map<Tk, Tv>
     */
    public static function exclude<Tk, Tv>(Map<Tk, Tv> $map, Vector<Tk> $keys): Map<Tk, Tv> {
        foreach ($keys as $key) {
            $map->remove($key);
        }

        return $map;
    }

    /**
     * Expand a singe-dimension map to a multi-dimensional map, where the values key is a dot notated path.
     *
     * @param Map<Tk, Tv> $map
     * @return Map<string, mixed>
     */
    public static function expand<Tk, Tv>(Map<Tk, Tv> $map): Map<string, mixed> {
        $data = Map {};

        foreach ($map as $key => $value) {
            $data = static::insert($data, (string) $key, $value);
        }

        return $data;
    }

    /**
     * Extract the value from a map, depending on the paths given, represented by dot notation.
     *
     * @param Map<Tk, mixed> $map
     * @param string $path
     * @return mixed
     */
    public static function extract<Tk>(Map<Tk, mixed> $map, string $path): mixed {
        $paths = explode('.', $path);
        $key = array_shift($paths);

        // Index does not exist
        if (!$map->contains($key)) {
            return null;
        }

        // Drill into the nested collection
        $value = $map[$key];

        if ($value instanceof Map && $paths) {
            return static::extract($value, implode('.', $paths));

        // Path goes deeper but the current value isn't a map
        } else if ($paths) {
            return null;
        }

        return $value;
    }

    /**
     * Flatten a multi-dimensional map by returning the values with their keys representing their previous pathing.
     *
     * @param Map<Tk, Tv> $map
     * @param string $path
     * @return Map<string, mixed>
     */
    public static function flatten<Tk, Tv>(Map<Tk, Tv> $map, string $path = ''): Map<string, mixed> {
        if ($path) {
            $path .= '.';
        }

        $data = Map {};

        foreach ($map as $key => $value) {
            $key = (string) $key;

            if ($value instanceof Map) {
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
     * Get a value from a map. If they path doesn't exist, return null, or if the path is empty, return the whole map.
     *
     * @param Map<Tk, mixed> $map
     * @param string $path
     * @return mixed
     */
    public static function get<Tk>(Map<Tk, mixed> $map, string $path): mixed {
        if ($path === '') {
            return $map; // Allow whole collection to be returned
        }

        return static::extract($map, $path);
    }

    /**
     * Checks to see if a key/value pair exists within a map, determined by the given path.
     *
     * @param Map<Tk, mixed> $map
     * @param string $path
     * @return bool
     */
    public static function has<Tk>(Map<Tk, mixed> $map, string $path): bool {
        $paths = explode('.', $path);
        $key = array_shift($paths);

        // Index does not exist
        if (!$map->contains($key)) {
            return false;
        }

        // Drill into the nested collection
        $value = $map[$key];

        if ($value instanceof Map && $paths) {
            return static::has($value, implode('.', $paths));
        }

        return true;
    }

    /**
     * Includes the specified key-value pair in the map if the key doesn't already exist.
     *
     * @param Map<Tk, mixed> $map
     * @param string $path
     * @param mixed $value
     * @return Map<Tk, mixed>
     */
    public static function inject<Tk>(Map<Tk, mixed> $map, string $path, mixed $value): Map<Tk, mixed> {
        if (static::has($map, $path)) {
            return $map;
        }

        return static::insert($map, $path, $value);
    }

    /**
     * Inserts a value into a map based on the given path.
     *
     * @param Map<Tk, mixed> $map
     * @param string $path
     * @param mixed $value
     * @return Map<Tk, mixed>
     */
    public static function insert<Tk>(Map<Tk, mixed> $map, string $path, mixed $value): Map<Tk, mixed> {
        $paths = explode('.', $path);
        $key = array_shift($paths);

        // In the last path so set the value
        if (!$paths) {
            $map[$key] = $value;

            return $map;
        }

        // Index does not exist
        if (!$map->contains($key) || !$map[$key] instanceof Map) {
            $map[$key] = Map {};
        }

        // Drill into the nested collection
        $item = $map[$key];

        invariant($item instanceof Map, 'Item is a Map');

        static::insert($item, implode('.', $paths), $value);

        return $map;
    }

    /**
     * Checks to see if all values in the collection are strings, returns false if not.
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
     * Checks to see if all values in the collection are numeric, returns false if not.
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
               return (string) $key;

            } else if ($value instanceof Indexish) {
                if ($nested = static::keyOf($value, $match)) {
                    $return .=  (string) $key . '.' . (string) $nested;
                }
            }
        }

        return trim($return, '.');
    }

    /**
     * Merge one map into another. Values from the secondary maps will overwrite the primary map.
     * If two values are maps, they will be merged recursively. If two values are vectors, they will be combined.
     *
     * @param Map<Tk, mixed> $base
     * @param Map<Tk, mixed> $merge
     * @return Map<Tk, mixed>
     */
    public static function merge<Tk>(Map<Tk, mixed> $base, ... $merges): Map<Tk, mixed> { // @todo - Variadic doesn't support type hints
        foreach ($merges as $merge) {
            if (!$merge instanceof Map) {
                continue;
            }

            foreach ($merge as $key => $value) {
                if ($base->contains($key)) {
                    $current = $base[$key];

                    if ($value instanceof Map && $current instanceof Map) {
                        $value = static::merge($current, $value);

                    } else if ($value instanceof Vector && $current instanceof Vector) {
                        $value = $current->addAll($value);
                    }
                }

                $base[$key] = $value;
            }
        }

        return $base;
    }

    /**
     * Pluck a value out of a map and return an vector of the plucked values.
     *
     * @param Map<Tk, Tv> $map
     * @param string $path
     * @return Vector<mixed>
     */
    public static function pluck<Tk, Tv>(Map<Tk, Tv> $map, string $path): Vector<mixed> {
        $data = Vector {};

        foreach ($map as $item) {
            if ($item instanceof Map) {
                if ($value = static::extract($item, $path)) {
                    $data[] = $value;
                }
            }
        }

        return $data;
    }

    /**
     * Reduce a map by removing all keys that have not been defined for persistence.
     *
     * @param Map<Tk, Tv> $map
     * @param Vector<Tk> $keys
     * @return Map<Tk, Tv>
     */
    public static function reduce<Tk, Tv>(Map<Tk, Tv> $map, Vector<Tk> $keys): Map<Tk, Tv> {
        $remove = Vector {};

        foreach ($map as $key => $value) {
            if (!in_array($key, $keys)) {
                $remove[] = $key;
            }
        }

        foreach ($remove as $key) {
            $map->remove($key);
        }

        return $map;
    }

    /**
     * Remove an key from a map, determined by the given path.
     *
     * @param Map<Tk, mixed> $map
     * @param string $path
     * @return Map<Tk, mixed>
     */
    public static function remove<Tk>(Map<Tk, mixed> $map, string $path): Map<Tk, mixed> {
        $paths = explode('.', $path);
        $key = array_shift($paths);

        // In the last path so remove the value
        if (!$paths) {
            $map->remove($key);
        }

        // Index does not exist
        if (!$map->contains($key) || !$map[$key] instanceof Map) {
            return $map;
        }

        // Drill into the nested collection
        $item = $map[$key];

        invariant($item instanceof Map, 'Item is a Map');

        static::remove($item, implode('.', $paths));

        return $map;
    }

    /**
     * Set a value into a map. If $paths is a map, loop over each item and insert the value.
     *
     * @param Map<Tk, mixed> $map
     * @param Map|string $path
     * @param mixed $value
     * @return Map<Tk, mixed>
     */
    public static function set<Tk>(Map<Tk, mixed> $map, mixed $path, mixed $value = null): Map<Tk, mixed> {
        if ($path instanceof Map) {
            foreach ($path as $key => $value) {
                $map = static::insert($map, $key, $value);
            }
        } else {
            $map = static::insert($map, (string) $path, $value);
        }

        return $map;
    }

    /**
     * Returns true if at least one element in the collection satisfies the provided testing function.
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
