<?php
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Utility;

use \Titon\Utility\Exception\UnsupportedMethodException;
use \Closure;

/**
 * Base class that all utility classes should extend.
 * Provides custom static magic methods through a macro interface.
 *
 * @package Titon\Utility
 */
class Macro {

    /**
     * Cached method results.
     *
     * @type array
     */
    protected static $_cache = array();

    /**
     * Custom methods to magically call.
     *
     * @type \Closure[]
     */
    protected static $_macros = array();

    /**
     * Execute a macro if it has been called statically.
     *
     * @param string $key
     * @param array $args
     * @return mixed
     * @throws \Titon\Utility\Exception\UnsupportedMethodException
     */
    public static function __callStatic($key, array $args) {
        $class = get_called_class();

        if (static::hasMacro($key)) {
            return call_user_func_array(static::$_macros[$class][$key], $args);
        }

        throw new UnsupportedMethodException(sprintf('Macro %s has not been defined for %s', $key, $class));
    }

    /**
     * Cache the result of an inflection by using a Closure.
     *
     * @param string|array $key
     * @param \Closure $callback
     * @return string
     */
    public static function cache($key, Closure $callback) {
        if (is_array($key)) {
            $key = implode('-', $key);
        }

        if (isset(static::$_cache[$key])) {
            return static::$_cache[$key];
        }

        static::$_cache[$key] = $callback();

        return static::$_cache[$key];
    }

    /**
     * Return true if a macro is defined by name.
     *
     * @param string $key
     * @return bool
     */
    public static function hasMacro($key) {
        return isset(static::$_macros[get_called_class()][$key]);
    }

    /**
     * Return true if the class method or a custom macro by name exists.
     *
     * @param string $key
     * @return bool
     */
    public static function hasMethod($key) {
        return (method_exists(get_called_class(), $key) || static::hasMacro($key));
    }

    /**
     * Define a custom macro, that will be triggered when a magic static method is called.
     *
     * @param string $key
     * @param \Closure $callback
     */
    public static function macro($key, Closure $callback) {
        static::$_macros[get_called_class()][$key] = $callback;
    }

    /**
     * Return all defined macros.
     *
     * @return \Closure[]
     */
    public static function macros() {
        $class = get_called_class();

        return isset(static::$_macros[$class]) ? static::$_macros[$class] : array();
    }

}