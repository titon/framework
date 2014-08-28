<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Common;

use Titon\Utility\Converter;
use Titon\Utility\Traverse;
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
     * @type Map<string, mixed>
     */
    protected Map<string, mixed> $_data = Map {};

    /**
     * Magic method for get().
     *
     * @param string $key
     * @return mixed
     */
    public function __get(string $key): mixed {
        return $this->get($key);
    }

    /**
     * Magic method for set().
     *
     * @param string $key
     * @param mixed $value
     */
    public function __set(string $key, mixed $value): void {
        $this->set($key, $value);
    }

    /**
     * Magic method for has().
     *
     * @param string $key
     * @return bool
     */
    public function __isset(string $key): bool {
        return $this->has($key);
    }

    /**
     * Magic method for remove().
     *
     * @param string $key
     */
    public function __unset(string $key): void {
        $this->remove($key);
    }

    /**
     * Add multiple parameters.
     *
     * @param Map<string, mixed> $params
     * @return $this
     */
    public function add(Map<string, mixed> $params): this {
        foreach ($params as $key => $value) {
            $this->set($key, $value);
        }

        return $this;
    }

    /**
     * Return all parameters.
     *
     * @return Map<string, mixed>
     */
    public function all(): Map<string, mixed> {
        return $this->_data;
    }

    /**
     * Reset all data.
     *
     * @return $this
     */
    public function flush(): this {
        $this->_data->clear();

        return $this;
    }

    /**
     * Return a parameter by key.
     *
     * @uses Titon\Utility\Traverse
     *
     * @param string $key
     * @param mixed $default
     * @return mixed
     */
    public function get(string $key, mixed $default = null): mixed {
        $value = Traverse::get($this->_data, $key);

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
    public function getIterator(): ArrayIterator {
        // todo - use collections iterator?
        return new ArrayIterator($this->_data->toArray());
    }

    /**
     * Check if a parameter exists.
     *
     * @uses Titon\Utility\Traverse
     *
     * @param string $key
     * @return bool
     */
    public function has(string $key): bool {
        return Traverse::has($this->_data, $key);
    }

    /**
     * Return all the parameter keys.
     *
     * @return Vector<string>
     */
    public function keys(): Vector<string> {
        return $this->_data->keys();
    }

    /**
     * Remove a parameter by key.
     *
     * @uses Titon\Utility\Traverse
     *
     * @param string $key
     * @return $this
     */
    public function remove(string $key): this {
        Traverse::remove($this->_data, $key);

        return $this;
    }

    /**
     * Set a parameter value by key.
     *
     * @uses Titon\Utility\Traverse
     *
     * @param string $key
     * @param mixed $value
     * @return $this
     */
    public function set(string $key, mixed $value = null): this {
        Traverse::set($this->_data, $key, $value);

        return $this;
    }

    /**
     * Return all the parameter values.
     *
     * @return Vector<mixed>
     */
    public function values(): Vector<mixed> {
        return $this->_data->values();
    }

    /**
     * Return the data as an array.
     *
     * @return array
     */
    public function toArray(): array {
        return Converter::toArray($this->all());
    }

    /**
     * Return the count of the array.
     *
     * @return int
     */
    public function count(): int {
        return $this->_data->count();
    }

}