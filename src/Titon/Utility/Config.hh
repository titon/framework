<?hh // strict
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
     * @type Map<string, mixed>
     */
    protected static Map<string, mixed> $_config = Map {};

    /**
     * Add a value to a key. If the value is not an array, make it one.
     *
     * @param string $key
     * @param mixed $value
     */
    public static function add(string $key, mixed $value): void {
        $data = Converter::toVector(static::get($key, []));
        $data[] = $value;

        static::set($key, $data);
    }

    /**
     * Return all configuration.
     *
     * @return Map<string, mixed>
     */
    public static function all(): Map<string, mixed> {
        return static::$_config;
    }

    /**
     * Get the currently defined encoding for the application.
     *
     * @return string
     */
    public static function encoding(): string {
        return static::get('app.encoding') ?: 'UTF-8';
    }

    /**
     * Flush configuration by removing all settings.
     */
    public static function flush(): void {
        static::$_config->clear();
    }

    /**
     * Grab a value from the current configuration.
     *
     * @param string $key
     * @param mixed $default
     * @return mixed
     */
    public static function get(string $key, ?mixed $default = null): ?mixed {
        $value = Traverse::get(static::$_config, $key);

        if ($value === null) {
            return $default;
        }

        return $value;
    }

    /**
     * Checks to see if a key exists within the current configuration.
     *
     * @param string $key
     * @return bool
     */
    public static function has(string $key): bool {
        return Traverse::has(static::$_config, $key);
    }

    /**
     * Loads a user created file into the configuration class.
     * Uses the defined reader to parse the file.
     *
     * @param string $key
     * @param \Titon\Io\Reader $reader
     */
    public static function load(string $key, Reader $reader): void {
        static::$_config[$key] = $reader->read();

        unset($reader);
    }

    /**
     * Grabs the defined project name.
     *
     * @return string
     */
    public static function name(): string {
        return static::get('app.name');
    }

    /**
     * Remove a value from the config.
     *
     * @param string $key
     */
    public static function remove(string $key): void {
        Traverse::remove(static::$_config, $key);
    }

    /**
     * Get the currently defined salt for the application.
     *
     * @return string
     */
    public static function salt(): string {
        return static::get('app.salt');
    }

    /**
     * Add values to the current loaded configuration.
     *
     * @param string $key
     * @param mixed $value
     */
    public static function set(string $key, ?mixed $value): void {
        Traverse::set(static::$_config, $key, $value);
    }

}
