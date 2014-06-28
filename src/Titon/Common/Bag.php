<?hh
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
     * @param array $data
     * @return $this
     */
    public function add(array $data);

    /**
     * Return all parameters and their values within the bag.
     *
     * @return array
     */
    public function all();

    /**
     * Remove all values within the bag.
     *
     * @return $this
     */
    public function flush();

    /**
     * Return a value defined by key, or by dot notated path.
     * If no key is found, return null, or if there is no value,
     * return the default value.
     *
     * @param string $key
     * @param mixed $default
     * @return string
     */
    public function get($key, $default = null);

    /**
     * Set a value defined by key. Can pass in a dot notated path
     * to insert into a nested structure.
     *
     * @param string $key
     * @param string $value
     * @return $this
     */
    public function set($key, $value);

    /**
     * Check if a key exists within the bag.
     * Can use a dot notated path as the key.
     *
     * @param string $key
     * @return bool
     */
    public function has($key);

    /**
     * Remove a value defined by key, or dot notated path.
     *
     * @param string $key
     * @return $this
     */
    public function remove($key);

}