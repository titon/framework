<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Utility;

use Titon\Common\Macroable;
use Titon\Type\Contract\Arrayable;
use Titon\Type\Contract\Mapable;
use Titon\Type\Contract\Vectorable;
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
     * @param Indexish<Tk, Tv> $collection
     * @param (function(Tk, Tv): Tv) $callback
     * @param bool $recursive
     * @return Map<Ta, Tb>
     */
    public static function each<Tk, Tv, Ta>(Indexish<Tk, Tv> $collection, (function(Tk, Tv): Ta) $callback, bool $recursive = true): Map<Tk, Ta> {
        $clean = Map {};

        foreach ($collection as $key => $value) {
            if ($value instanceof Indexish && $recursive) {
                $clean[$key] = static::each($value, $callback, $recursive);
            } else {
                $clean[$key] = call_user_func_array($callback, [$key, $value]);
            }
        }

        return $clean;
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
            $data = static::insert($data, (string) $key, $value);
        }

        return $data;
    }

    /**
     * Extract the value from a map, depending on the paths given, represented by dot notation.
     *
     * @param Map<Tk, Tv> $map
     * @param string $path
     * @return Tv
     */
    public static function extract<Tk, Tv>(Map<Tk, Tv> $map, string $path): ?Tv {
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
     * @param Indexish<Tk, Tv> $map
     * @param string $path
     * @return Map<string, mixed>
     */
    public static function flatten<Tk, Tv>(Indexish<Tk, Tv> $map, string $path = ''): Map<Tk, Tv> {
        if ($path) {
            $path .= '.';
        }

        $data = Map {};

        foreach ($map as $key => $value) {
            $stringKey = $path . (string) $key;

            if ($value instanceof Indexish) {
                if (!$value) {
                    $data[$stringKey] = null;
                } else {
                    $data = static::merge($data, static::flatten($value, $stringKey));
                }
            } else {
                $data[$stringKey] = $value;
            }
        }

        return $data;
    }

    /**
     * Get a value from a map. If they path doesn't exist, return null, or if the path is empty, return the whole map.
     *
     * @param Map<Tk, Tv> $map
     * @param string $path
     * @param Tv $default
     * @return Tv
     */
    public static function get<Tk, Tv>(Map<Tk, Tv> $map, string $path, ?Tv $default = null): ?Tv {
        if ($path === '') {
            return $map; // Allow whole collection to be returned
        }

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
     * @param string $path
     * @return bool
     */
    public static function has<Tk, Tv>(Map<Tk, Tv> $map, string $path): bool {
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
     * @param Map<Tk, Tv> $map
     * @param string $path
     * @param Tv $value
     * @return Map<Tk, Tv>
     */
    public static function inject<Tk, Tv>(Map<Tk, Tv> $map, string $path, Tv $value): Map<Tk, Tv> {
        if (static::has($map, $path)) {
            return $map;
        }

        return static::insert($map, $path, $value);
    }

    /**
     * Inserts a value into a map based on the given path.
     *
     * @param Map<Tk, Tv> $map
     * @param string $path
     * @param Tv $value
     * @return Map<Tk, Tv>
     */
    public static function insert<Tk, Tv>(Map<Tk, Tv> $map, string $path, Tv $value): Map<Tk, Tv> {
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
     * @param Map<Tk, Tv> $base
     * @param Map<Tk, Tv> $merge
     * @return Map<Tk, Tv>
     */
    public static function merge<Tk, Tv>(Map<Tk, Tv> $base, ...$merges): Map<Tk, Tv> { // @todo - Variadic doesn't support type hints
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
     * @return Vector<Tv>
     */
    public static function pluck<Tk, Tv>(Map<Tk, Tv> $map, string $path): Vector<Tv> {
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
     * @param string $path
     * @return Map<Tk, Tv>
     */
    public static function remove<Tk, Tv>(Map<Tk, Tv> $map, string $path): Map<Tk, Tv> {
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

        invariant($item instanceof Map, 'Item must be a Map');

        static::remove($item, implode('.', $paths));

        return $map;
    }

    /**
     * Set a value into a map. If $paths is a map, loop over each item and insert the value.
     *
     * @param Map<Tk, mixed> $map
     * @param Map|string $path
     * @param Tv $value
     * @return Map<Tk, Tv>
     */
    public static function set<Tk, Tv>(Map<Tk, Tv> $map, mixed $path, ?Tv $value = null): Map<Tk, Tv> {
        if ($path instanceof Map) {
            foreach ($path as $key => $value) {
                $map = static::insert($map, (string) $key, $value);
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

    /**
     * Recursively convert a resource into an array.
     *
     * @param mixed $resource
     * @return array<Tk, Tv>
     */
    public static function toArray<Tk, Tv>(mixed $resource): array<Tk, Tv> {
        if ($resource instanceof Arrayable) {
            return $resource->toArray();
        }

        $array = [];

        if ($resource instanceof Indexish) {
            foreach ($resource as $key => $value) {
                if ($value instanceof Indexish) {
                    $array[$key] = static::toArray($value);
                } else {
                    $array[$key] = $value;
                }
            }
        } else {
            $array[] = $resource;
        }

        return $array;
    }

    /**
     * Recursively convert a resource into a map.
     *
     * @param mixed $resource
     * @return Map<Tk, Tv>
     */
    public static function toMap<Tk, Tv>(mixed $resource): Map<Tk, Tv> {
        $map = Map {};

        if ($resource instanceof Mapable) {
            return $resource->toMap();

        } else if (!$resource instanceof KeyedTraversable) {
            $map[0] = $resource;

            return $map;
        }

        invariant($resource instanceof KeyedTraversable, 'Resource must be traversable');

        foreach ($resource as $key => $value) {
            if ($value instanceof Vector) {
                $map[$key] = static::toVector($value);

            } else if ($value instanceof KeyedTraversable) {
                $map[$key] = static::toMap($value);

            } else {
                $map[$key] = $value;
            }
        }

        return $map;
    }

    /**
     * Recursively convert a resource into a vector.
     *
     * @param mixed $resource
     * @return Vector<Tv>
     */
    public static function toVector<Tv>(mixed $resource): Vector<Tv> {
        $vector = Vector {};

        if ($resource instanceof Vectorable) {
            return $resource->toVector();

        } else if (!$resource instanceof KeyedTraversable) {
            $vector[] = $resource;

            return $vector;
        }

        foreach ($resource as $value) {
            if ($value instanceof Map || (is_array($value) && !Col::isNumeric(array_keys($value)))) {
                $vector[] = static::toMap($value);

            } else if ($value instanceof KeyedTraversable) {
                $vector[] = static::toVector($value);

            } else {
                $vector[] = $value;
            }
        }

        return $vector;
    }


}
