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
interface Bag {

    /**
     * Add multiple parameters that will overwrite any previously defined parameters.
     *
     * @param Map<string, mixed> $data
     * @return $this
     */
    public function add(Map<string, mixed> $data): this;

    /**
     * Return all parameters and their values within the bag.
     *
     * @return Map<string, mixed>
     */
    public function all(): this;

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
     * @param string $key
     * @param mixed $default
     * @return mixed
     */
    public function get(string $key, mixed $default = null): mixed;

    /**
     * Set a value defined by key. Can pass in a dot notated path
     * to insert into a nested structure.
     *
     * @param string $key
     * @param mixed $value
     * @return $this
     */
    public function set(string $key, mixed $value): this;

    /**
     * Check if a key exists within the bag.
     * Can use a dot notated path as the key.
     *
     * @param string $key
     * @return bool
     */
    public function has(string $key): bool;

    /**
     * Remove a value defined by key, or dot notated path.
     *
     * @param string $key
     * @return $this
     */
    public function remove(string $key): this;

}