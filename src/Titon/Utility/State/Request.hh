<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Utility\State;

use Titon\Utility\Col;
use Titon\Utility\Converter;

type GlobalMap = Map<string, mixed>;

/**
 * The Request class acts as a static immutable wrapper for the $_REQUEST super global.
 * The class also acts a base for all other super globals to inherit from.
 *
 * @package Titon\Utility\State
 */
abstract class Request {

    /**
     * Super global data collection.
     *
     * @type Map<string, \Titon\Utility\State\GlobalMap>
     */
    protected static Map<string, GlobalMap> $_data = Map {};

    /**
     * Has the super global data been initialized?
     *
     * @type Map<string, bool>
     */
    protected static Map<string, bool> $_loaded = Map {};

    /**
     * Return the entire data collection.
     *
     * @return \Titon\Utility\State\GlobalMap
     */
    public static function all(): GlobalMap {
        return static::$_data->get(static::class) ?: Map {};
    }

    /**
     * Return a value based on a dot notated path. If no value is found, return a default value.
     *
     * @param string $key
     * @param mixed $default
     * @return mixed
     */
    public static function get(string $key, mixed $default = null): mixed {
        $value = Col::get(static::$_data, static::class . '.' . $key);

        if ($value === null) {
            return $default;
        }

        return $value;
    }

    /**
     * Return true if a key exists within the collection.
     * Supports a dot notated path for nested collections.
     *
     * @param string $key
     * @return bool
     */
    public static function has(string $key): bool {
        return Col::has(static::$_data, static::class . '.' . $key);
    }

    /**
     * Initialize the class by supplying an array of data that recursively gets converted to a map.
     * That data should only be initialized once, unless it is running through the command line.
     *
     * @param array<string, mixed> $data
     */
    public static function initialize(array<string, mixed> $data): void {
        $class = static::class;

        if (php_sapi_name() === 'cli' || !static::$_loaded->contains($class)) {
            static::$_data[$class] = static::package($data);
            static::$_loaded[$class] = true;
        }
    }

    /**
     * Package the data into a format usable by the framework, primarily a map with string keys.
     *
     * @param array<string, mixed> $data
     * @return \Titon\Utility\State\GlobalMap
     */
    public static function package(array<string, mixed> $data): GlobalMap {
        return Converter::toMap($data);
    }

}