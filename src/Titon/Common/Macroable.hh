<?hh

namespace Titon\Common;

use \Titon\Utility\Exception\UnsupportedMethodException;
use \Closure;

type CallbackFunction = (function(): mixed);

trait Macroable {

    /**
     * Custom methods to magically call through the static context.
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
    public static function __callStatic(string $key, array $args): mixed {
        if (static::hasMacro($key)) {
            return call_user_func_array(static::getMacros()->get($key), $args);
        }

        throw new UnsupportedMethodException(sprintf('Macro %s has not been defined for %s', $key, static::class));
    }

    /**
     * Return all defined macros for a class.
     *
     * @return Map<string, CallbackFunction>
     */
    public static function getMacros(): Map<string, CallbackFunction> {
        $macros = static::$_macros;
        $class = static::class;

        if (!$macros->contains($class)) {
            $macros->set($class, Map {});
        }

        return $macros->get($class);
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
     * @param \Closure $callback
     */
    public static function macro(string $key, Closure $callback): void {
        static::getMacros()->set($key, $callback);
    }

}