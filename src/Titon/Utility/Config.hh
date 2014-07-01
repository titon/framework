<?hh
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Utility;

use Titon\Common\Macroable;
use Titon\Io\Reader;

/**
 * Stores the current configuration options for the application.
 * Configuration can be loaded from multiple sources including environment, bootstraps and internal system classes.
 * Various readers can be used to import specific configuration files.
 *
 * @package Titon\Utility
 */
class Config {
    use Macroable;

    /**
     * Current loaded configuration.
     *
     * @type array
     */
    protected static Map<string, mixed> $_config = Map {};

    /**
     * Add a value to a key. If the value is not an array, make it one.
     *
     * @param string $key
     * @param mixed $value
     */
    public static function add($key, $value) {
        $data = (array) static::get($key, []);
        $data[] = $value;

        static::set($key, $data);
    }

    /**
     * Return all configuration.
     *
     * @return array
     */
    public static function all() {
        return static::$_config;
    }

    /**
     * Get the currently defined encoding for the application.
     *
     * @return string
     */
    public static function encoding() {
        return static::get('app.encoding') ?: 'UTF-8';
    }

    /**
     * Flush configuration by removing all settings.
     */
    public static function flush() {
        static::$_config->clear();
    }

    /**
     * Grab a value from the current configuration.
     *
     * @param string $key
     * @param mixed $default
     * @return mixed
     */
    public static function get($key, $default = null) {
        $value = Traverse::get(static::$_config, $key);

        if ($value === null) {
            return $default;
        }

        return $value;
    }

    /**
     * Checks to see if a key exists within the current configuration.
     *
     * @uses Titon\Utility\Hash
     *
     * @param string $key
     * @return bool
     */
    public static function has($key) {
        return Traverse::has(static::$_config, $key);
    }

    /**
     * Loads a user created file into the configuration class.
     * Uses the defined reader to parse the file.
     *
     * @param string $key
     * @param \Titon\Io\Reader $reader
     */
    public static function load($key, Reader $reader) {
        static::$_config[$key] = $reader->read();

        unset($reader);
    }

    /**
     * Grabs the defined project name.
     *
     * @return string
     */
    public static function name() {
        return static::get('app.name');
    }

    /**
     * Remove a value from the config.
     *
     * @uses Titon\Utility\Hash
     *
     * @param string $key
     */
    public static function remove($key) {
        static::$_config = Traverse::remove(static::$_config, $key);
    }

    /**
     * Get the currently defined salt for the application.
     *
     * @return string
     */
    public static function salt() {
        return static::get('app.salt');
    }

    /**
     * Add values to the current loaded configuration.
     *
     * @uses Titon\Utility\Hash
     *
     * @param string $key
     * @param mixed $value
     */
    public static function set($key, $value) {
        static::$_config = Traverse::set(static::$_config, $key, $value);
    }

}
