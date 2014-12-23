<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Common;

use \Titon\Utility\Exception\UnsupportedMethodException;

type MacroCallback = (function(): mixed);
type MacroContainer = Map<string, MacroMap>;
type MacroMap = Map<string, MacroCallback>;

/**
 * Provides a mechanism at runtime for defining static methods that can be triggered during __callStatic().
 *
 * @package Titon\Common
 */
trait Macroable {

    /**
     * Custom methods to magically call through the static context.
     *
     * @var \Titon\Common\MacroContainer
     */
    protected static MacroContainer $_macros = Map {};

    /**
     * Execute a macro if it has been called statically.
     *
     * @param string $key
     * @param array $args
     * @return mixed
     * @throws \Titon\Utility\Exception\UnsupportedMethodException
     */
    public static function __callStatic(string $key, array<mixed> $args): mixed {
        if (static::hasMacro($key)) {
            return call_user_func_array(static::getMacros()->get($key), $args);
        }

        throw new UnsupportedMethodException(sprintf('Macro %s has not been defined for %s', $key, static::class));
    }

    /**
     * Return all defined macros for a class.
     *
     * @return \Titon\Common\MacroMap
     */
    public static function getMacros(): MacroMap {
        $macros = static::$_macros;
        $class = static::class;

        if (!$macros->contains($class)) {
            $macros[$class] = Map {};
        }

        return $macros[$class];
    }

    /**
     * Return true if a macro is defined by name.
     *
     * @param string $key
     * @return bool
     */
    public static function hasMacro(string $key): bool {
        return static::getMacros()->contains($key);
    }

    /**
     * Define a custom macro, that will be triggered when a magic static method is called.
     *
     * @param string $key
     * @param \Titon\Common\MacroCallback $callback
     */
    public static function macro(string $key, MacroCallback $callback): void {
        static::getMacros()->set($key, $callback);
    }

}