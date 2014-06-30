<?hh
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Utility;

use \Titon\Utility\Exception\UnsupportedMethodException;
use \Closure;

type CallbackFunction = (function(): mixed);

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
     * @type Map<string, CallbackFunction>
     */
    protected static Map<string, CallbackFunction> $_cache = Map {};

    /**
     * Custom methods to magically call.
     *
     * @type Map<string, Map<string, CallbackFunction>>
     */
    protected static Map<string, Map<string, CallbackFunction>> $_macros = Map {};

    /**
     * Execute a macro if it has been called statically.
     *
     * @param string $key
     * @param array $args
     * @return mixed
     * @throws \Titon\Utility\Exception\UnsupportedMethodException
     */
    public static function __callStatic(string $key, array $args): void {
        if (static::hasMacro($key)) {
            return call_user_func_array(static::macros()->get($key), $args);
        }

        throw new UnsupportedMethodException(sprintf('Macro %s has not been defined for %s', $key, static::class));
    }

    /**
     * Cache the result of an inflection by using a Closure.
     *
     * @param string|array $key
     * @param \Closure $callback
     * @return mixed
     */
    public static function cache(mixed $key, Closure $callback): mixed {
        if (is_array($key)) {
            $key = implode('-', $key);
        }

        if (static::$_cache->contains($key)) {
            return static::$_cache->get($key);
        }

        static::$_cache->set($key, $callback());

        return static::$_cache->get($key);
    }

    /**
     * Return true if a macro is defined by name.
     *
     * @param string $key
     * @return bool
     */
    public static function hasMacro(string $key): bool {
        return static::macros()->contains($key);
    }

    /**
     * Return true if the class method or a custom macro by name exists.
     *
     * @param string $key
     * @return bool
     */
    public static function hasMethod(string $key): bool {
        return (method_exists(static::class, $key) || static::hasMacro($key));
    }

    /**
     * Define a custom macro, that will be triggered when a magic static method is called.
     *
     * @param string $key
     * @param \Closure $callback
     */
    public static function macro(string $key, Closure $callback): void {
        static::macros()->set($key, $callback);
    }

    /**
     * Return all defined macros for a class.
     *
     * @return Map<string, CallbackFunction>
     */
    public static function macros(): Map<string, CallbackFunction> {
        $macros = static::$_macros;
        $class = static::class;

        if (!$macros->contains($class)) {
            $macros->set($class, new Map());
        }

        return $macros->get($class);
    }

}