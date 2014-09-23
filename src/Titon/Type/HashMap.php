<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Type;

use Titon\Type\Contract\Arrayable;
use Titon\Type\Contract\Jsonable;
use Titon\Type\Contract\Xmlable;
use Titon\Type\Exception\InvalidFormatException;
use Titon\Utility\Converter;
use Titon\Utility\Hash;
use \Closure;
use \ArrayIterator;
use \IteratorAggregate;
use \Countable;

/**
 * The Map type allows for the modification, manipulation and traversal of an array through the use of an object like interface.
 * One can also modify the map using a series of chained method calls that sequentially alter the initial value.
 *
 * @package Titon\Type
 */
class HashMap extends Type implements Countable, IteratorAggregate, Arrayable, Jsonable, Xmlable {

    /**
     * Add a value to the end of the array. This does not support literal keys.
     *
     * @param mixed $value
     * @return $this
     */
    public function append($value) {
        if (is_array($value)) {
            foreach ($value as $v) {
                $this->append($v);
            }
        } else {
            array_push($this->_value, $value);
        }

        return $this;
    }

    /**
     * Split an array into chunks.
     *
     * @param int $size
     * @param bool $preserve
     * @return array
     */
    public function chunk($size, $preserve = true) {
        return array_chunk($this->value(), (int) $size, $preserve);
    }

    /**
     * Removes all empty, null, false and 0 items.
     *
     * @return $this
     */
    public function clean() {
        $data = $this->value();

        foreach ($data as $key => $value) {
            if (!$value && $value !== 0) {
                unset($data[$key]);
            }
        }

        return new static($data);
    }

    /**
     * Compares the current array against the passed array and returns a new array
     * with all the values that are found within both arrays. If strict is true,
     * the keys must match as well as the values. A callback can be passed to
     * further filter down the results.
     *
     * If options.on equals "keys":
     * Compares the current array against the passed array and returns a new array
     * with all the values where keys are matched in both arrays.
     * Only differences from the class instance is returned.
     *
     * If options.valueCallback is set:
     * Works exactly like default compare() except that it uses a callback to validate the values.
     * A second callback can be used to also compared against the array key.
     *
     * @uses Titon\Utility\Hash
     *
     * @param array $array
     * @param mixed $options {
     *      @type bool $strict              Will validate the array keys as well
     *      @type \Closure $callback        Closure to compare keys with
     *      @type \Closure $valueCallback   Closure to compare values with
     *      @type string $on                Either "keys" or "values"
     * }
     * @return array
     */
    public function compare(array $array, array $options = []) {
        $options = $options + [
            'strict' => true,
            'callback' => null,
            'valueCallback' => null,
            'on' => 'values'
        ];

        $callback = $options['callback'];
        $valueCallback = $options['valueCallback'];

        // Prepare array
        $value = Hash::filter($this->value(), false, function($val) {
            return !is_array($val);
        });

        // Values
        if ($options['on'] === 'values') {

            // Compare with callback
            if ($valueCallback instanceof Closure) {
                if ($callback instanceof Closure) {
                    $output = array_uintersect_uassoc($value, $array, $valueCallback, $callback);

                } else if ($options['strict']) {
                    $output = array_uintersect_assoc($value, $array, $valueCallback);

                } else {
                    $output = array_uintersect($value, $array, $valueCallback);
                }

            // Compare regular
            } else {
                if ($options['strict']) {
                    if ($callback instanceof Closure) {
                        $output = array_intersect_uassoc($value, $array, $callback);

                    } else {
                        $output = array_intersect_assoc($value, $array);
                    }
                } else {
                    $output = array_intersect($value, $array);
                }
            }

        // Keys
        } else {
            if ($callback instanceof Closure) {
                $output = array_intersect_ukey($value, $array, $callback);

            } else {
                $output = array_intersect_key($value, $array);
            }
        }

        return new static($output);
    }

    /**
     * Merges the passed array with the current array and returns a new Map object.
     *
     * @uses Titon\Utility\Hash
     *
     * @param array $array
     * @return $this
     */
    public function concat(array $array) {
        return new static(Hash::merge($this->value(), $array));
    }

    /**
     * Checks if a value exists in the array.
     *
     * @param mixed $value
     * @return bool
     */
    public function contains($value) {
        return in_array($value, array_values($this->value()), true);
    }

    /**
     * Counts all the values in the array.
     *
     * @return int
     */
    public function countValues() {
        $values = array_values($this->value());

        foreach ($values as $key => $value) {
            if (!is_string($value) && !is_numeric($value)) {
                unset($values[$key]);
            }
        }

        return array_count_values($values);
    }

    /**
     * Determines how deep the nested array is.
     *
     * @uses Titon\Utility\Hash
     *
     * @return int
     */
    public function depth() {
        return Hash::depth($this->value());
    }

    /**
     * Compares the current array against the passed array and returns a new array
     * with all the values that are not found within the passed array. If strict is true,
     * the keys must match as well as the values. A callback can be passed to
     * further filter down the results.
     *
     * If options.on equals "keys":
     * Compares the current array against the passed array and returns a new array
     * with all the values where keys are not matched in both arrays.
     * Only differences from the class instance is returned.
     *
     * If options.valueCallback is set:
     * Works exactly like default difference() except that it uses a callback to validate the values.
     * A second callback can be used to also compared against the array key.
     *
     * @uses Titon\Utility\Hash
     *
     * @param array $array
     * @param mixed $options {
     *      @type bool $strict              Will validate the array keys as well
     *      @type \Closure $callback        Closure to compare keys with
     *      @type \Closure $valueCallback   Closure to compare values with
     *      @type string $on                Either "keys" or "values"
     * }
     * @return array
     */
    public function difference(array $array, array $options = []) {
        $options = $options + [
            'strict' => true,
            'callback' => null,
            'valueCallback' => null,
            'on' => 'values'
        ];

        $callback = $options['callback'];
        $valueCallback = $options['valueCallback'];

        // Prepare array
        $value = Hash::filter($this->value(), false, function($val) {
            return !is_array($val);
        });

        // Values
        if ($options['on'] === 'values') {

            // Compare with callback
            if ($valueCallback instanceof Closure) {
                if ($callback instanceof Closure) {
                    $output = array_udiff_uassoc($value, $array, $valueCallback, $callback);

                } else if ($options['strict']) {
                    $output = array_udiff_assoc($value, $array, $valueCallback);

                } else {
                    $output = array_udiff($value, $array, $valueCallback);
                }

            // Compare regular
            } else {
                if ($options['strict']) {
                    if ($callback instanceof Closure) {
                        $output = array_diff_uassoc($value, $array, $callback);

                    } else {
                        $output = array_diff_assoc($value, $array);
                    }
                } else {
                    $output = array_diff($value, $array);
                }
            }

            // Keys
        } else {
            if ($callback instanceof Closure) {
                $output = array_diff_ukey($value, $array, $callback);

            } else {
                $output = array_diff_key($value, $array);
            }
        }

        return new static($output);
    }

    /**
     * Apply a user function to every member of an array.
     *
     * @uses Titon\Utility\Hash
     *
     * @param \Closure $callback
     * @param bool $recursive
     * @return $this
     */
    public function each(Closure $callback, $recursive = true) {
        return new static(Hash::each($this->value(), $callback, $recursive));
    }

    /**
     * Checks to see if the passed argument is an explicit exact match.
     *
     * @param mixed $value
     * @return bool
     */
    public function equals($value) {
        return ($this->value() === $value);
    }

    /**
     * Removes all occurrences of an item from the array.
     *
     * @param mixed $erase
     * @return $this
     */
    public function erase($erase) {
        $data = $this->value();

        foreach ($data as $key => $value) {
            if ($value === $erase) {
                unset($data[$key]);
            }
        }

        return new static($data);
    }

    /**
     * Returns true if every element in the array satisfies the provided testing function.
     *
     * @uses Titon\Utility\Hash
     *
     * @param \Closure $callback
     * @return bool
     */
    public function every(Closure $callback) {
        return Hash::every($this->value(), $callback);
    }

    /**
     * Extracts a value from the specified index. Accepts a dot notated path to filter down the depth.
     *
     * @uses Titon\Utility\Hash
     *
     * @param string $key
     * @return mixed
     */
    public function extract($key) {
        return Hash::get($this->value(), $key);
    }

    /**
     * Filters elements of the array using a callback function.
     *
     * @uses Titon\Utility\Hash
     *
     * @param \Closure $callback
     * @param bool $recursive
     * @return $this
     */
    public function filter(Closure $callback = null, $recursive = true) {
        return new static(Hash::filter($this->value(), $recursive, $callback));
    }

    /**
     * Attempt to find an object whose property matches a specific value.
     * Can optionally pass a custom finder callback.
     *
     * @param mixed $search
     * @param string $key
     * @return mixed
     */
    public function find($search, $key = 'id') {
        $callback = $this->_buildFinder($key, $search);

        foreach ($this->value() as $value) {
            if ($callback($value)) {
                return $value;
            }
        }

        return null;
    }

    /**
     * Attempt to find multiple objects whose property matches a specific value.
     * Can optionally pass a custom finder callback.
     *
     * @param mixed $search
     * @param string $key
     * @return $this
     */
    public function findMany($search, $key = 'id') {
        $callback = $this->_buildFinder($key, $search);

        return new static($this->filter($callback, false)->values());
    }

    /**
     * Return the first element in the array.
     *
     * @return mixed
     */
    public function first() {
        return $this->isNotEmpty() ? reset($this->_value) : null;
    }

    /**
     * Flattens a multidimensional array into a single array.
     *
     * @uses Titon\Utility\Hash
     *
     * @return $this
     */
    public function flatten() {
        return new static(Hash::flatten($this->value()));
    }

    /**
     * Exchanges all keys with their associated values in the array.
     *
     * @uses Titon\Utility\Hash
     *
     * @param bool $recursive
     * @param bool $truncate
     * @return $this
     */
    public function flip($recursive = true, $truncate = true) {
        return new static(Hash::flip($this->value(), $recursive, $truncate));
    }

    /**
     * Empty the array.
     *
     * @return $this
     */
    public function flush() {
        $this->_value = [];

        return $this;
    }

    /**
     * Grab a value based on a single key. Returns by reference to support objects in ArrayAccess.
     * Use extract() to go further than a single key deep.
     *
     * @param string $key
     * @return mixed
     */
    public function get($key) {
        return isset($this->_value[$key]) ? $this->_value[$key] : null;
    }

    /**
     * Return an array iterator.
     *
     * @return \ArrayIterator
     */
    public function getIterator() {
        return new ArrayIterator($this->value());
    }

    /**
     * Group all values into sub-groups based on the value of a specific key.
     *
     * @param string $key
     * @return array
     */
    public function groupBy($key) {
        $grouped = [];
        $class = get_class($this);

        foreach ($this->value() as $value) {
            $grouped[$value[$key]][] = $value;
        }

        return array_map(function($value) use ($class) {
            return new $class($value);
        }, $grouped);
    }

    /**
     * Checks to see if a certain index exists. Accepts a dot notated path to filter down the depth.
     *
     * @uses Titon\Utility\Hash
     *
     * @param string $key
     * @return bool
     */
    public function has($key) {
        return Hash::has($this->value(), $key);
    }

    /**
     * Returns the index in which the passed key exists. Validates against literal and numeric keys.
     *
     * @param mixed $key
     * @return int
     */
    public function indexOf($key) {
        $count = 0;

        if ($this->isNotEmpty()) {
            foreach ($this->value() as $index => $value) {
                if ($index === $key) {
                    return $count;
                }

                ++$count;
            }
        }

        return -1;
    }

    /**
     * Checks to see if the array is empty.
     *
     * @return bool
     */
    public function isEmpty() {
        return (count($this->_value) === 0);
    }

    /**
     * Checks to see if the array is not empty.
     *
     * @return bool
     */
    public function isNotEmpty() {
        return count($this->_value);
    }

    /**
     * Return the values for JSON serialization.
     *
     * @return array
     */
    public function jsonSerialize() {
        return $this->toArray();
    }

    /**
     * Return all the keys or a subset of the keys of the array.
     *
     * @return array
     */
    public function keys() {
        return array_keys($this->value());
    }

    /**
     * Return the last element in the array.
     *
     * @return mixed
     */
    public function last() {
        return $this->isNotEmpty() ? end($this->_value) : null;
    }

    /**
     * Return the length of the array.
     *
     * @return int
     */
    public function length() {
        return count($this->value());
    }

    /**
     * Applies the callback to the elements of the array.
     *
     * @param \Closure $callback
     * @return $this
     */
    public function map(Closure $callback) {
        return new static(array_map($callback, $this->value()));
    }

    /**
     * Merge an array with the current array.
     *
     * @uses Titon\Utility\Hash
     *
     * @param array $array
     * @return $this
     */
    public function merge(array $array) {
        return new static(Hash::merge($this->value(), $array));
    }

    /**
     * Pluck the value of a specific field from each entity.
     *
     * @param string $key
     * @return array
     */
    public function pluck($key = 'id') {
        return array_map(function($value) use ($key) {
            return $value[$key];
        }, $this->value());
    }

    /**
     * Add a value to the beginning of the array. This will reset all numerical indices.
     *
     * @param mixed $value
     * @return $this
     */
    public function prepend($value) {
        if (is_array($value)) {
            foreach ($value as $v) {
                $this->prepend($v);
            }
        } else {
            array_unshift($this->_value, $value);
        }

        return $this;
    }

    /**
     * Calculate the product of values in the array.
     *
     * @return int
     */
    public function product() {
        return array_product($this->value());
    }

    /**
     * Returns a random item from the array.
     *
     * @return mixed
     */
    public function random() {
        $values = array_values($this->value());
        $random = rand(0, count($values) - 1);

        return $values[$random];
    }

    /**
     * Iteratively reduce the array to a single value using a callback function.
     *
     * @param \Closure $callback
     * @param mixed $initial
     * @return int
     */
    public function reduce(Closure $callback, $initial = null) {
        return array_reduce($this->value(), $callback, $initial);
    }

    /**
     * Remove an index from the array. Accepts a dot notated path to drill down the dimensions.
     *
     * @uses Titon\Utility\Hash
     *
     * @param string $key
     * @return $this
     */
    public function remove($key) {
        $this->_value = Hash::remove($this->value(), $key);

        return $this;
    }

    /**
     * Reverse the order of the array. If preserve is true, keys will not be reset.
     *
     * @param bool $preserve
     * @return $this
     */
    public function reverse($preserve = true) {
        return new static(array_reverse($this->value(), $preserve));
    }

    /**
     * Return map as a serialized array.
     *
     * @return string
     */
    public function serialize() {
        return $this->to('serialize');
    }

    /**
     * Set a key/value pair within in any multi-dimensional array depth.
     *
     * @uses Titon\Utility\Hash
     *
     * @param string $key
     * @param mixed $value
     * @return $this
     */
    public function set($key, $value) {
        $this->_value = Hash::set($this->value(), $key, $value);

        return $this;
    }

    /**
     * Randomize the order of elements in the array.
     *
     * @return $this
     */
    public function shuffle() {
        $data = $this->value();
        shuffle($data);

        return new static($data);
    }

    /**
     * Extract a slice of the array.
     *
     * @param int $offset
     * @param int $length
     * @param bool $preserve
     * @return array
     */
    public function slice($offset, $length = null, $preserve = true) {
        if (!$length && $length !== 0) {
            $length = abs($offset);
        }

        return new static(array_slice($this->value(), (int) $offset, (int) $length, $preserve));
    }

    /**
     * Returns true if at least one element in the array satisfies the provided testing function.
     *
     * @uses Titon\Utility\Hash
     *
     * @param \Closure $callback
     * @return bool
     */
    public function some(Closure $callback) {
        return Hash::some($this->value(), $callback);
    }

    /**
     * Sort the values in the array based on the supplied options.
     *
     * @param mixed $options {
     *      @type bool $reverse         Will reverse sort
     *      @type bool $preserve        Indices will be left in tact
     *      @type int $flags            Sorting flags
     *      @type \Closure $callback    Closure callback to sort with
     *      @type string $on            Either "keys" or "values"
     * }
     * @return $this
     */
    public function sort($options = []) {
        if ($options instanceof Closure) {
            $options = ['callback' => $options];

        } else if (is_bool($options)) {
            $options = ['reverse' => $options];
        }

        $options = $options + [
            'reverse' => false,
            'preserve' => true,
            'flags' => SORT_REGULAR,
            'callback' => null,
            'on' => 'values'
        ];

        $values = $this->value();
        $flags = $options['flags'];
        $preserve = $options['preserve'];

        // Sort by callback
        if ($options['callback'] instanceof Closure) {

            // Sort keys by callback
            if ($options['on'] === 'keys') {
                uksort($values, $options['callback']);

            // Sort values by callback
            } else {
                if ($preserve) {
                    uasort($values, $options['callback']);
                } else {
                    usort($values, $options['callback']);
                }
            }

        // Sort regular
        } else {

            // Sort by keys
            if ($options['on'] === 'keys') {
                ksort($values, $flags);

            // Sort by values
            } else {
                if ($preserve) {
                    asort($values, $flags);
                } else {
                    sort($values, $flags);
                }
            }
        }

        // Reverse it
        if ($options['reverse']) {
            $values = array_reverse($values, $preserve);
        }

        return new static($values);
    }

    /**
     * Sort a multi-dimensional array by a field within each value.
     *
     * @param string|\Closure $key
     * @param int $flags
     * @param bool $reverse
     * @return $this
     */
    public function sortBy($key, $flags = SORT_REGULAR, $reverse = false) {
        if ($key instanceof Closure) {
            $callback = $key;
        } else {
            $callback = function($a, $b) use ($key) {
                return strcmp($a[$key], $b[$key]);
            };
        }

        return $this->sort([
            'reverse' => $reverse,
            'preserve' => false,
            'flags' => $flags,
            'callback' => $callback
        ]);
    }

    /**
     * Sort the array using a natural algorithm. This function implements a sort algorithm that orders
     * alphanumeric strings in the way a human being would while maintaining key/value associations.
     *
     * @param bool $strict
     * @return $this
     */
    public function sortNatural($strict = false) {
        $values = $this->value();

        if ($strict) {
            natsort($values);
        } else {
            natcasesort($values);
        }

        return new static($values);
    }

    /**
     * Remove a portion of the array and replace it with something else; will preserve keys.
     *
     * @uses Titon\Utility\Hash
     *
     * @param int $offset
     * @param int $length
     * @param array $replacement
     * @return array
     */
    public function splice($offset, $length, array $replacement) {
        $before = [];
        $after = [];
        $splice = [];
        $i = 0;
        $l = 0;

        foreach ($this->value() as $key => $value) {
            if ($i >= $offset && $l < $length) {
                $splice[$key] = $value;
                $l++;

            } else if ($i < $offset) {
                $before[$key] = $value;

            } else {
                $after[$key] = $value;
            }

            $i++;
        }

        return new static(Hash::merge($before, $replacement, $after));
    }

    /**
     * Calculate the sum of values in the array.
     *
     * @return int
     */
    public function sum() {
        return array_sum($this->value());
    }

    /**
     * Convert the entire map to a specific format.
     * Each objects in the map must implement any required interfaces.
     *
     * @param string $format
     * @param mixed $options
     * @return string|array
     * @throws \Titon\Type\Exception\InvalidFormatException
     */
    public function to($format, $options = null) {
        switch ($format) {
            case 'array':
                return Converter::toArray($this->value(), true);
            break;
            case 'json':
                return Converter::toJson($this->value(), $options);
            break;
            case 'xml':
                return Converter::toXml($this->value(), $options ?: 'collection');
            break;
            case 'serialize':
                return Converter::toSerialize($this->value());
            break;
            default:
                if (is_callable($format)) {
                    return array_map($format, $this->value());
                }
            break;
        }

        throw new InvalidFormatException(sprintf('Invalid format %s for collection conversion', $format));
    }

    /**
     * {@inheritdoc}
     */
    public function toArray() {
        return $this->to('array');
    }

    /**
     * {@inheritdoc}
     */
    public function toJson($options = 0) {
        return $this->to('json', $options);
    }

    /**
     * Define basic to string.
     *
     * @return string
     */
    public function toString() {
        return $this->serialize();
    }

    /**
     * {@inheritdoc}
     */
    public function toXml($root = 'collection') {
        return $this->to('xml', $root);
    }

    /**
     * Removes duplicate values from the array.
     *
     * @param int $flags
     * @return $this
     */
    public function unique($flags = SORT_REGULAR) {
        return new static(array_unique($this->value(), $flags));
    }

    /**
     * Return all the values of an array and reorder indices.
     *
     * @return array
     */
    public function values() {
        return array_values($this->value());
    }

    /**
     * {@inheritdoc}
     */
    public function write($value) {
        return parent::write((array) $value);
    }

    /**
     * Countable: Return the length of the array.
     *
     * @return bool
     */
    public function count() {
        return $this->length();
    }

    /**
     * Prepare the callback to use for finding.
     *
     * @param string $key
     * @param mixed $search
     * @return callable
     */
    protected function _buildFinder($key, $search) {
        if ($search instanceof Closure) {
            return $search;
        }

        return function($value) use ($search, $key) {
            if (is_array($search)) {
                return in_array($value[$key], $search);
            }

            return ($value[$key] == $search);
        };
    }

}
