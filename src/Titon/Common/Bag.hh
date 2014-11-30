<?hh // strict
/**
 * @copyright   2010-2014, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Common;

/**
 * A bag can be used for managing sets of specialized data.
 *
 * @package Titon\Common
 */
interface Bag<Tk, Tv> {

    /**
     * Add multiple parameters that will overwrite any previously defined parameters.
     *
     * @param Map<Tk, Tv> $data
     * @return $this
     */
    public function add(Map<Tk, Tv> $data): this;

    /**
     * Return all parameters and their values within the bag.
     *
     * @return Map<Tk, Tv>
     */
    public function all(): Map<Tk, Tv>;

    /**
     * Remove all values within the bag.
     *
     * @return $this
     */
    public function flush(): this;

    /**
     * Return a value defined by key, or by dot notated path.
     * If no key is found, return null, or if there is no value,
     * return the default value.
     *
     * @param Tk $key
     * @param Tv $default
     * @return Tv
     */
    public function get(Tk $key, ?Tv $default = null): ?Tv;

    /**
     * Check if a key exists within the bag.
     * Can use a dot notated path as the key.
     *
     * @param Tk $key
     * @return bool
     */
    public function has(Tk $key): bool;

    /**
     * Remove a value defined by key, or dot notated path.
     *
     * @param Tk $key
     * @return $this
     */
    public function remove(Tk $key): this;

    /**
     * Set a value defined by key. Can pass in a dot notated path
     * to insert into a nested structure.
     *
     * @param Tk $key
     * @param Tv $value
     * @return $this
     */
    public function set(Tk $key, Tv $value): this;

}