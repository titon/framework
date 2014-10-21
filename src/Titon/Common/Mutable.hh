<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Common;

use Titon\Utility\Col;

/**
 * The Mutable trait allows for a set of data to be read and written to.
 * It also provides methods for array access, iteration, and counting.
 * This trait is useful when an object should represent a set of data or an entity.
 *
 * @package Titon\Common
 */
trait Mutable<Tk, Tv> {

    /**
     * Mapped data.
     *
     * @type Map<Tk, Tv>
     */
    protected Map<Tk, Tv> $_data = Map {};

    /**
     * Magic method for get().
     *
     * @param Tk $key
     * @return Tv
     */
    public function __get(Tk $key): ?Tv {
        return $this->get($key);
    }

    /**
     * Magic method for set().
     *
     * @param Tk $key
     * @param Tv $value
     */
    public function __set(Tk $key, Tv $value): void {
        $this->set($key, $value);
    }

    /**
     * Magic method for has().
     *
     * @param Tk $key
     * @return bool
     */
    public function __isset(Tk $key): bool {
        return $this->has($key);
    }

    /**
     * Magic method for remove().
     *
     * @param Tk $key
     */
    public function __unset(Tk $key): void {
        $this->remove($key);
    }

    /**
     * Add multiple parameters.
     *
     * @param Map<Tk, Tv> $params
     * @return $this
     */
    public function add(Map<Tk, Tv> $params): this {
        foreach ($params as $key => $value) {
            $this->set($key, $value);
        }

        return $this;
    }

    /**
     * Return all parameters.
     *
     * @return Map<Tk, Tv>
     */
    public function all(): Map<Tk, Tv> {
        return $this->_data;
    }

    /**
     * Reset all data.
     *
     * @return $this
     */
    public function flush(): this {
        $this->all()->clear();

        return $this;
    }

    /**
     * Return a parameter by key.
     *
     * @uses Titon\Utility\Col
     *
     * @param Tk $key
     * @param Tv $default
     * @return Tv
     */
    public function get(Tk $key, ?Tv $default = null): ?Tv {
        $value = Col::get($this->all(), (string) $key);

        if ($value === null) {
            return $default;
        }

        return $value;
    }

    /**
     * Return an iterator.
     *
     * @return Iterator<Tv>
     */
    public function getIterator(): Iterator<Tv> {
        return $this->all()->getIterator();
    }

    /**
     * Check if a parameter exists.
     *
     * @uses Titon\Utility\Col
     *
     * @param Tk $key
     * @return bool
     */
    public function has(Tk $key): bool {
        return Col::has($this->all(), (string) $key);
    }

    /**
     * Return all the parameter keys.
     *
     * @return Vector<Tk>
     */
    public function keys(): Vector<Tk> {
        return $this->all()->keys();
    }

    /**
     * Remove a parameter by key.
     *
     * @uses Titon\Utility\Col
     *
     * @param Tk $key
     * @return $this
     */
    public function remove(Tk $key): this {
        Col::remove($this->all(), (string) $key);

        return $this;
    }

    /**
     * Set a parameter value by key.
     *
     * @uses Titon\Utility\Col
     *
     * @param Tk $key
     * @param Tv $value
     * @return $this
     */
    public function set(Tk $key, Tv $value): this {
        Col::set($this->all(), (string) $key, $value);

        return $this;
    }

    /**
     * Return all the parameter values.
     *
     * @return Vector<Tv>
     */
    public function values(): Vector<Tv> {
        return $this->all()->values();
    }

    /**
     * Return the data as an array.
     *
     * @return array<Tk, Tv>
     */
    public function toArray(): array<Tk, Tv> {
        return $this->all()->toArray();
    }

    /**
     * Return the count of the collection.
     *
     * @return int
     */
    public function count(): int {
        return $this->all()->count();
    }

}