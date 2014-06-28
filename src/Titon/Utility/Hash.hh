<?hh
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Utility;

use Titon\Utility\Exception\InvalidTypeException;
use \Closure;

/**
 * Manipulates, manages and processes multiple types of result sets: objects and arrays.
 *
 * @package Titon\Utility
 */
class Hash extends Macro {

    /**
     * Determines the total depth of a multi-dimensional array or object.
     * Has two methods of determining depth: based on recursive depth, or based on tab indentation (faster).
     *
     * @uses Titon\Utility\Converter
     *
     * @param array|object $set
     * @return int
     * @throws \Titon\Utility\Exception\InvalidTypeException
     */
    public static function depth($set) {
        if (is_object($set)) {
            $set = Converter::toArray($set);

        } else if (!is_array($set)) {
            throw new InvalidTypeException('Value passed must be an array');
        }

        if (!$set) {
            return 0;
        }

        $depth = 1;

        foreach ($set as $value) {
            if (is_array($value)) {
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
     * @param array $set
     * @param \Closure $callback
     * @param bool $recursive
     * @return array
     */
    public static function each(array $set, Closure $callback, $recursive = true) {
        foreach ($set as $key => $value) {
            if (is_array($value) && $recursive) {
                $set[$key] = static::each($value, $callback, $recursive);
            } else {
                $set[$key] = $callback($value, $key);
            }
        }

        return $set;
    }

    /**
     * Returns true if every element in the array satisfies the provided testing function.
     *
     * @param array $set
     * @param \Closure $callback
     * @return bool
     */
    public static function every(array $set, Closure $callback) {
        foreach ($set as $key => $value) {
            if (!$callback($value, $key)) {
                return false;
            }
        }

        return true;
    }

    /**
     * Exclude specific keys from the array and return the new array.
     *
     * @param array $set
     * @param array $keys
     * @return array
     */
    public static function exclude(array $set, array $keys) {
        foreach ($keys as $key) {
            unset($set[$key]);
        }

        return $set;
    }

    /**
     * Expand an array to a fully workable multi-dimensional array, where the values key is a dot notated path.
     *
     * @param array $set
     * @return array
     */
    public static function expand(array $set) {
        $data = [];

        foreach ($set as $key => $value) {
            $data = static::insert($data, $key, $value);
        }

        return $data;
    }

    /**
     * Extract the value of an array, depending on the paths given, represented by key.key.key notation.
     *
     * @param array $set
     * @param string $path
     * @return mixed
     */
    public static function extract(array $set, $path) {
        if (!$set) {
            return null;
        }

        // Exit early for faster processing
        if (strpos($path, '.') === false) {
            return isset($set[$path]) ? $set[$path] : null;
        }

        $search =& $set;
        $paths = explode('.', (string) $path);
        $total = count($paths);

        while ($total > 0) {
            $key = $paths[0];

            // Within the last path
            if ($total === 1) {
                return array_key_exists($key, $search) ? $search[$key] : null;

            // Break out of non-existent paths early
            } else if (!array_key_exists($key, $search) || !is_array($search[$key])) {
                break;
            }

            $search =& $search[$key];
            array_shift($paths);
            $total--;
        }

        return null;
    }

    /**
     * Filter out all keys within an array that have an empty value, excluding 0 (string and numeric).
     * If $recursive is set to true, will remove all empty values within all sub-arrays.
     *
     * @param array $set
     * @param bool $recursive
     * @param \Closure $callback
     * @return array
     */
    public static function filter(array $set, $recursive = true, Closure $callback = null) {
        if ($recursive) {
            foreach ($set as $key => $value) {
                if (is_array($value)) {
                    $set[$key] = static::filter($value, $recursive);
                }
            }
        }

        if ($callback === null) {
            $callback = function($var) {
                return ($var === 0 || $var === '0' || !empty($var));
            };
        }

        return array_filter($set, $callback);
    }

    /**
     * Flatten a multi-dimensional array by returning the values with their keys representing their previous pathing.
     *
     * @param array $set
     * @param string $path
     * @return array
     */
    public static function flatten(array $set, $path = null) {
        if ($path) {
            $path = $path . '.';
        }

        $data = [];

        foreach ($set as $key => $value) {
            if (is_array($value)) {
                if ($value) {
                    $data += static::flatten($value, $path . $key);
                } else {
                    $data[$path . $key] = null;
                }
            } else {
                $data[$path . $key] = $value;
            }
        }

        return $data;
    }

    /**
     * Flip the array by replacing all array keys with their value, if the value is a string and the key is numeric.
     * If the value is empty/false/null and $truncate is true, that key will be removed.
     *
     * @param array $set
     * @param bool $recursive
     * @param bool $truncate
     * @return array
     */
    public static function flip(array $set, $recursive = true, $truncate = true) {
        $data = [];

        foreach ($set as $key => $value) {
            $empty = ($value === '' || $value === false || $value === null);

            if (is_array($value)) {
                if ($recursive) {
                    $data[$key] = static::flip($value, $truncate);
                }

            } else if (is_int($key) && !$empty) {
                $data[$value] = '';

            } else {
                if ($truncate && !$empty) {
                    $data[$value] = $key;
                }
            }
        }

        return $data;
    }

    /**
     * Get a value from the set. If they path doesn't exist, return null, or if the path is empty, return the whole set.
     *
     * @param array $set
     * @param string $path
     * @return mixed
     */
    public static function get(array $set, $path = null) {
        if (!$path) {
            return $set;
        }

        return static::extract($set, $path);
    }

    /**
     * Checks to see if a key/value pair exists within an array, determined by the given path.
     *
     * @param array $set
     * @param string $path
     * @return bool
     */
    public static function has(array $set, $path) {
        if (!$set || !$path) {
            return false;
        }

        // Exit early for faster processing
        if (strpos($path, '.') === false) {
            return isset($set[$path]);
        }

        $search =& $set;
        $paths = explode('.', (string) $path);
        $total = count($paths);

        while ($total > 0) {
            $key = $paths[0];

            // Within the last path
            if ($total === 1) {
                return array_key_exists($key, $search);

            // Break out of non-existent paths early
            } else if (!array_key_exists($key, $search) || !is_array($search[$key])) {
                break;
            }

            $search =& $search[$key];
            array_shift($paths);
            $total--;
        }

        return false;
    }

    /**
     * Includes the specified key-value pair in the set if the key doesn't already exist.
     *
     * @param array $set
     * @param string $path
     * @param mixed $value
     * @return array
     */
    public static function inject(array $set, $path, $value) {
        if (static::has($set, $path)) {
            return $set;
        }

        return static::insert($set, $path, $value);
    }

    /**
     * Inserts a value into the array set based on the given path.
     *
     * @param array $set
     * @param string $path
     * @param mixed $value
     * @return array
     */
    public static function insert(array $set, $path, $value) {
        if (!$path) {
            return $set;
        }

        // Exit early for faster processing
        if (strpos($path, '.') === false) {
            $set[$path] = $value;

            return $set;
        }

        $search =& $set;
        $paths = explode('.', $path);
        $total = count($paths);

        while ($total > 0) {
            $key = $paths[0];

            // Within the last path
            if ($total === 1) {
                $search[$key] = $value;

            // Break out of non-existent paths early
            } else if (!array_key_exists($key, $search) || !is_array($search[$key])) {
                $search[$key] = [];
            }

            $search =& $search[$key];
            array_shift($paths);
            $total--;
        }

        unset($search);

        return $set;
    }

    /**
     * Checks to see if all values in the array are strings, returns false if not.
     * If $strict is true, method will fail if there are values that are numerical strings, but are not cast as integers.
     *
     * @param array $set
     * @param bool $strict
     * @return bool
     */
    public static function isAlpha(array $set, $strict = true) {
        return static::every($set, function($value) use ($strict) {
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
     * @param array $set
     * @return bool
     */
    public static function isNumeric(array $set) {
        return static::every($set, function($value) {
            return is_numeric($value);
        });
    }

    /**
     * Returns the key of the specified value. Will recursively search if the first pass doesn't match.
     *
     * @param array $set
     * @param mixed $match
     * @return mixed
     */
    public static function keyOf(array $set, $match) {
        $return = null;
        $isArray = [];

        foreach ($set as $key => $value) {
            if ($value === $match) {
                $return = $key;
            }

            if (is_array($value)) {
                $isArray[] = $key;
            }
        }

        if (!$return && $isArray) {
            foreach ($isArray as $key) {
                if ($value = static::keyOf($set[$key], $match)) {
                    $return = $key . '.' . $value;
                }
            }
        }

        return $return;
    }

    /**
     * Works in a similar fashion to array_map() but can be used recursively as well as supply arguments for the function callback.
     * Additionally, the $function argument can be a string or array containing the class and method name.
     *
     * @param array $set
     * @param string|\Closure $function
     * @param array $args
     * @return array
     */
    public static function map(array $set, $function, $args = []) {
        foreach ($set as $key => $value) {
            if (is_array($value)) {
                $set[$key] = static::map($value, $function, $args);

            } else {
                $temp = $args;
                array_unshift($temp, $value);

                $set[$key] = call_user_func_array($function, $temp);
            }
        }

        return $set;
    }

    /**
     * Compares to see if the first array set matches the second set exactly.
     *
     * @param array $set1
     * @param array $set2
     * @return bool
     */
    public static function matches(array $set1, array $set2) {
        return ($set1 === $set2);
    }

    /**
     * Merge is a combination of array_merge() and array_merge_recursive(). However, when merging two keys with the same key,
     * the previous value will be overwritten instead of being added into an array. The later array takes precedence when merging.
     *
     * @param array $array,...
     * @return array
     */
    public static function merge() {
        $sets = func_get_args();
        $data = [];

        if (!$sets) {
            return $data;
        }

        foreach ($sets as $set) {
            foreach ((array) $set as $key => $value) {
                if (isset($data[$key])) {
                    if (is_array($value) && is_array($data[$key])) {
                        $data[$key] = static::merge($data[$key], $value);

                    } else {
                        $data[$key] = $value;
                    }
                } else {
                    $data[$key] = $value;
                }
            }
        }

        return $data;
    }

    /**
     * Works similar to merge(), except that it will only overwrite/merge values if the keys exist in the previous array.
     *
     * @param array $set1 - The base array
     * @param array $set2 - The array to overwrite the base array
     * @return array
     */
    public static function overwrite(array $set1, array $set2) {
        $overwrite = array_intersect_key($set2, $set1);

        if ($overwrite) {
            foreach ($overwrite as $key => $value) {
                if (is_array($value)) {
                    $set1[$key] = static::overwrite($set1[$key], $value);
                } else {
                    $set1[$key] = $value;
                }
            }
        }

        return $set1;
    }

    /**
     * Pluck a value out of each child-array and return an array of the plucked values.
     *
     * @param array $set
     * @param string $path
     * @return array
     */
    public static function pluck(array $set, $path) {
        $data = [];

        foreach ($set as $array) {
            if ($value = static::extract($array, $path)) {
                $data[] = $value;
            }
        }

        return $data;
    }

    /**
     * Generate an array with a range of numbers. Can apply a step interval to increase/decrease with larger increments.
     *
     * @param int $start
     * @param int $stop
     * @param int $step
     * @param bool $index
     * @return array
     */
    public static function range($start, $stop, $step = 1, $index = true) {
        $array = [];

        if ($stop > $start) {
            for ($i = $start; $i <= $stop; $i += $step) {
                if ($index) {
                    $array[$i] = $i;
                } else {
                    $array[] = $i;
                }
            }

        } else if ($stop < $start) {
            for ($i = $start; $i >= $stop; $i -= $step) {
                if ($index) {
                    $array[$i] = $i;
                } else {
                    $array[] = $i;
                }
            }
        }

        return $array;
    }

    /**
     * Reduce an array by removing all keys that have not been defined for persistence.
     *
     * @param array $set
     * @param array $keys
     * @return array
     */
    public static function reduce(array $set, array $keys) {
        $array = [];

        foreach ($set as $key => $value) {
            if (in_array($key, $keys)) {
                $array[$key] = $value;
            }
        }

        return $array;
    }

    /**
     * Remove an index from the array, determined by the given path.
     *
     * @param array $set
     * @param string $path
     * @return array
     */
    public static function remove(array $set, $path) {
        if (!$path) {
            return $set;
        }

        // Exit early for faster processing
        if (strpos($path, '.') === false) {
            unset($set[$path]);

            return $set;
        }

        $search =& $set;
        $paths = explode('.', (string) $path);
        $total = count($paths);

        while ($total > 0) {
            $key = $paths[0];

            // Within the last path
            if ($total === 1) {
                unset($search[$key]);
                return $set;

            // Break out of non-existent paths early
            } else if (!array_key_exists($key, $search) || !is_array($search[$key])) {
                break;
            }

            $search =& $search[$key];
            array_shift($paths);
            $total--;
        }

        return $set;
    }

    /**
     * Set a value into the result set. If the paths is an array, loop over each one and insert the value.
     *
     * @param array $set
     * @param array|string $path
     * @param mixed $value
     * @return array
     */
    public static function set(array $set, $path, $value = null) {
        if (is_array($path)) {
            foreach ($path as $key => $value) {
                $set = static::insert($set, $key, $value);
            }
        } else {
            $set = static::insert($set, $path, $value);
        }

        return $set;
    }

    /**
     * Returns true if at least one element in the array satisfies the provided testing function.
     *
     * @param array $set
     * @param \Closure $callback
     * @return bool
     */
    public static function some(array $set, Closure $callback) {
        foreach ($set as $value) {
            if ($callback($value, $value)) {
                return true;
            }
        }

        return false;
    }

}
