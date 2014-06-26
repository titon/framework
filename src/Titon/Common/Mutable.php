<?php
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Common;

use Titon\Utility\Converter;
use Titon\Utility\Hash;
use \ArrayIterator;

/**
 * The Mutable trait allows for a set of data to be read and written to.
 * It also provides methods for array access, iteration, and counting.
 * This trait is useful when an object should represent a set of data or an entity.
 *
 * @package Titon\Common
 */
trait Mutable {

    /**
     * Mapped data.
     *
     * @type array
     */
    protected $_data = [];

    /**
     * Magic method for get().
     *
     * @param string $key
     * @return mixed
     */
    public function __get($key) {
        return $this->get($key);
    }

    /**
     * Magic method for set().
     *
     * @param string $key
     * @param mixed $value
     */
    public function __set($key, $value) {
        $this->set($key, $value);
    }

    /**
     * Magic method for has().
     *
     * @param string $key
     * @return bool
     */
    public function __isset($key) {
        return $this->has($key);
    }

    /**
     * Magic method for remove().
     *
     * @param string $key
     */
    public function __unset($key) {
        $this->remove($key);
    }

    /**
     * Add multiple parameters.
     *
     * @param array $params
     * @return $this
     */
    public function add(array $params) {
        foreach ($params as $key => $value) {
            $this->set($key, $value);
        }

        return $this;
    }

    /**
     * Return all parameters.
     *
     * @return array
     */
    public function all() {
        return $this->_data;
    }

    /**
     * Reset all data.
     *
     * @return $this
     */
    public function flush() {
        $this->_data = [];

        return $this;
    }

    /**
     * Return a parameter by key.
     *
     * @uses Titon\Utility\Hash
     *
     * @param string $key
     * @param mixed $default
     * @return mixed
     */
    public function get($key, $default = null) {
        $value = Hash::get($this->_data, $key);

        if ($value === null) {
            return $default;
        }

        return $value;
    }

    /**
     * Return an iterator.
     *
     * @return \ArrayIterator
     */
    public function getIterator() {
        return new ArrayIterator($this->_data);
    }

    /**
     * Check if a parameter exists.
     *
     * @uses Titon\Utility\Hash
     *
     * @param string $key
     * @return bool
     */
    public function has($key) {
        return Hash::has($this->_data, $key);
    }

    /**
     * Return all the parameter keys.
     *
     * @return array
     */
    public function keys() {
        return array_keys($this->_data);
    }

    /**
     * Remove a parameter by key.
     *
     * @uses Titon\Utility\Hash
     *
     * @param string $key
     * @return $this
     */
    public function remove($key) {
        $this->_data = Hash::remove($this->_data, $key);

        return $this;
    }

    /**
     * Set a parameter value by key.
     *
     * @uses Titon\Utility\Hash
     *
     * @param string $key
     * @param mixed $value
     * @return $this
     */
    public function set($key, $value = null) {
        $this->_data = Hash::set($this->_data, $key, $value);

        return $this;
    }

    /**
     * Return the data as an array.
     *
     * @return array
     */
    public function toArray() {
        return Converter::toArray($this->all());
    }

    /**
     * Alias method for get().
     *
     * @param string $key
     * @return mixed
     */
    public function offsetGet($key) {
        return $this->get($key);
    }

    /**
     * Alias method for set().
     *
     * @param string $key
     * @param mixed $value
     */
    public function offsetSet($key, $value) {
        $this->set($key, $value);
    }

    /**
     * Alias method for has().
     *
     * @param string $key
     * @return bool
     */
    public function offsetExists($key) {
        return $this->has($key);
    }

    /**
     * Alias method for remove().
     *
     * @param string $key
     */
    public function offsetUnset($key) {
        $this->remove($key);
    }

    /**
     * Return the count of the array.
     *
     * @return int
     */
    public function count() {
        return count($this->_data);
    }

}