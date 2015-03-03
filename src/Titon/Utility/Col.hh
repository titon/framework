<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Utility;

use Titon\Common\Macroable;
use Titon\Common\Arrayable;
use Titon\Common\Mapable;
use Titon\Common\Vectorable;
use \HH\Traversable;
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
     *
     * @param Indexish<Tk, Tv> $collection
     * @param (function(Tk, Tv): Tv) $callback
     * @return Indexish<Tk, Tv>
     */
    public static function each<Tk, Tv>(Indexish<Tk, Tv> $collection, (function(Tk, Tv): Tv) $callback): Indexish<Tk, Tv> {
        foreach ($collection as $key => $value) {
            $collection[$key] = call_user_func_array($callback, [$key, $value]);
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
     * @return Map<Tk, Tv>
     */
    public static function expand<Tk, Tv>(Map<Tk, Tv> $map): Map<Tk, Tv> {
        $data = Map {};

        foreach ($map as $key => $value) {
            $data = static::insert($data, $key, $value);
        }

        return $data;
    }

    /**
     * Extract the value from a map, depending on the paths given, represented by dot notation.
     *
     * @param Map<Tk, Tv> $map
     * @param Tk $path
     * @return Tv
     */
    public static function extract<Tk, Tv>(Map<Tk, Tv> $map, Tk $path): ?Tv {
        $paths = explode('.', (string) $path);
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
     * @param Indexish<Tk, Tv> $map
     * @param string $path
     * @return Map<Tk, Tv>
     */
    public static function flatten<Tk, Tv>(Indexish<Tk, Tv> $map, string $path = ''): Map<Tk, Tv> {
        if ($path) {
            $path .= '.';
        }

        $data = Map {};

        foreach ($map as $key => $value) {
            $stringKey = $path . (string) $key;

            if ($value instanceof Indexish) {
                if (count($value) <= 0) {
                    $data[$stringKey] = null;
                } else {
                    $data = static::merge($data, static::flatten($value, $stringKey));
                }
            } else {
                $data[$stringKey] = $value;
            }
        }

        // UNSAFE
        // Because we are returning a map with a key of type `string` instead of `Tk`
        return $data;
    }

    /**
     * Get a value from a map. If the path doesn't exist, return null, or a default value.
     *
     * @param Map<Tk, Tv> $map
     * @param Tk $path
     * @param Tv $default
     * @return Tv
     */
    public static function get<Tk, Tv>(Map<Tk, Tv> $map, Tk $path, ?Tv $default = null): ?Tv {
        $value = static::extract($map, $path);

        if ($value === null) {
            return $default;
        }

        return $value;
    }

    /**
     * Checks to see if a key/value pair exists within a map, determined by the given path.
     *
     * @param Map<Tk, Tv> $map
     * @param Tk $path
     * @return bool
     */
    public static function has<Tk, Tv>(Map<Tk, Tv> $map, Tk $path): bool {
        $paths = explode('.', (string) $path);
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
     * @param Map<Tk, Tv> $map
     * @param Tk $path
     * @param Tv $value
     * @return Map<Tk, Tv>
     */
    public static function inject<Tk, Tv>(Map<Tk, Tv> $map, Tk $path, Tv $value): Map<Tk, Tv> {
        if (static::has($map, $path)) {
            return $map;
        }

        return static::insert($map, $path, $value);
    }

    /**
     * Inserts a value into a map based on the given path.
     *
     * @param Map<Tk, Tv> $map
     * @param Tk $path
     * @param Tv $value
     * @return Map<Tk, Tv>
     */
    public static function insert<Tk, Tv>(Map<Tk, Tv> $map, Tk $path, Tv $value): Map<Tk, Tv> {
        $paths = explode('.', (string) $path);
        $key = array_shift($paths);

        // In the last path so set the value
        if (!$paths) {
            $map[$key] = $value;

            return $map;
        }

        // Index does not exist
        if (!$map->contains($key) || !$map[$key] instanceof Map) {
            // UNSAFE
            // Since we are setting a `Map` instead of `Tv`
            $map[$key] = Map {};
        }

        // Drill into the nested collection
        $item = $map[$key];

        invariant($item instanceof Map, 'Item must be a Map');

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
        return static::every($collection, ($key, $value) ==> {
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
        return static::every($collection, ($key, $value) ==> is_numeric($value) );
    }

    /**
     * Returns the key of the specified value. Will recursively search if the first pass doesn't match.
     *
     * @param Indexish<Tk, Tv> $collection
     * @param Tm $match
     * @return string
     */
    public static function keyOf<Tk, Tv, Tm>(Indexish<Tk, Tv> $collection, Tm $match): string {
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
     * @param Map<Tk, Tv> $merges
     * @return Map<Tk, Tv>
     */
    public static function merge<Tk, Tv>(/* HH_FIXME[4033]: variadic + strict */ ...$merges): Map<Tk, Tv> {
        $base = Map {};

        foreach ($merges as $merge) {
            if (!$merge instanceof Indexish) {
                continue;
            }

            foreach ($merge as $key => $value) {
                if ($base->contains($key)) {
                    $current = $base[$key];

                    if ($value instanceof Vector && $current instanceof Vector) {
                        $value = $current->addAll($value);

                    } else if ($value instanceof Indexish && $current instanceof Indexish) {
                        $value = static::merge($current, $value); // Map and array
                    }
                }

                $base[$key] = $value;
            }
        }

        // UNSAFE
        // Since we are returning values of `Map` instead of `Tv`
        return $base;
    }

    /**
     * Pluck a value out of a map and return an vector of the plucked values.
     *
     * @param Map<Tk, Tv> $map
     * @param Tk $path
     * @return Vector<Tv>
     */
    public static function pluck<Tk, Tv>(Map<Tk, Tv> $map, Tk $path): Vector<Tv> {
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
     * @param Map<Tk, Tv> $map
     * @param Tk $path
     * @return Map<Tk, Tv>
     */
    public static function remove<Tk, Tv>(Map<Tk, Tv> $map, Tk $path): Map<Tk, Tv> {
        $paths = explode('.', (string) $path);
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

        invariant($item instanceof Map, 'Item must be a Map');

        static::remove($item, implode('.', $paths));

        return $map;
    }

    /**
     * Set a value into a map. If $paths is a map, loop over each item and insert the value.
     *
     * @param Map<Tk, Tv> $map
     * @param Tk $path
     * @param Tv $value
     * @return Map<Tk, Tv>
     */
    public static function set<Tk, Tv>(Map<Tk, Tv> $map, Tk $path, ?Tv $value = null): Map<Tk, Tv> {
        if ($path instanceof Map) {
            foreach ($path as $key => $value) {
                $map = static::insert($map, $key, $value);
            }

        } else if ($value !== null) {
            $map = static::insert($map, $path, $value);
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

    /**
     * Recursively convert a resource into an array.
     *
     * @param Tr $resource
     * @return array<Tk, Tv>
     */
    public static function toArray<Tk, Tv, Tr>(Tr $resource): array<Tk, Tv> {
        if ($resource instanceof Arrayable) {
            return $resource->toArray();

        } else if (!$resource instanceof Indexish) {
            // UNSAFE
            // Since we are returning an `int` instead of `Tk`
            return [$resource];
        }

        invariant($resource instanceof Indexish, 'Resource must be traversable');

        $array = [];

        foreach ($resource as $key => $value) {
            if ($value instanceof Indexish) {
                $array[$key] = static::toArray($value);

            } else {
                $array[$key] = $value;
            }
        }

        return $array;
    }

    /**
     * Recursively convert a resource into a map.
     *
     * @param Tr $resource
     * @return Map<Tk, Tv>
     */
    public static function toMap<Tk, Tv, Tr>(Tr $resource): Map<Tk, Tv> {
        if ($resource instanceof Mapable) {
            return $resource->toMap();

        } else if (!$resource instanceof Indexish) {
            // UNSAFE
            // Since we are returning a `Tr` instead of `Tv`
            return new Map([$resource]);
        }

        invariant($resource instanceof Indexish, 'Resource must be traversable');

        $map = Map {};

        foreach ($resource as $key => $value) {
            if ($value instanceof Vector || (is_array($value) && $value && Col::isNumeric(array_keys($value)))) {
                $map[$key] = static::toVector($value);

            } else if ($value instanceof Indexish) {
                $map[$key] = static::toMap($value);

            } else {
                $map[$key] = $value;
            }
        }

        // UNSAFE
        // Since the values in the map are more than just `Tv`
        return $map;
    }

    /**
     * Recursively convert a resource into a vector.
     *
     * @param Tr $resource
     * @return Vector<Tv>
     */
    public static function toVector<Tv, Tr>(Tr $resource): Vector<Tv> {
        if ($resource instanceof Vectorable) {
            return $resource->toVector();

        } else if (!$resource instanceof Indexish) {
            // UNSAFE
            // Since we are returning a `Tr` instead of `Tv`
            return new Vector([$resource]);
        }

        invariant($resource instanceof Indexish, 'Resource must be traversable');

        $vector = Vector {};

        foreach ($resource as $value) {
            if ($value instanceof Map || (is_array($value) && !Col::isNumeric(array_keys($value)))) {
                $vector[] = static::toMap($value);

            } else if ($value instanceof Indexish) {
                $vector[] = static::toVector($value);

            } else {
                $vector[] = $value;
            }
        }

        // UNSAFE
        // Since the values in the vector are more than just `Tv`
        return $vector;
    }

}
