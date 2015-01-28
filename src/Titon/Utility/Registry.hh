<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Utility;

use Titon\Common\Macroable;
use Titon\Common\ArgumentList;
use Titon\Utility\Exception\InvalidObjectException;
use Titon\Utility\Exception\MissingObjectException;
use \ReflectionClass;

type RegistryCallback<T> = (function(): T);
type RegistryMap<T> = Map<string, T>;

/**
 * The Registry acts a central hub where any part of the application can access a single instance of a stored object.
 * It registers all objects into the class to store in memory and be re-usable later at any given time.
 *
 * @package Titon\Utility
 */
class Registry<T> {
    use Macroable;

    /**
     * Objects that have been registered into memory. The array index is represented by the namespace convention,
     * where as the array value would be the matching instantiated object.
     *
     * @var \Titon\Utility\RegistryMap
     */
    protected static RegistryMap<T> $registered = Map {};

    /**
     * Return all registered objects.
     *
     * @return \Titon\Utility\RegistryMap
     */
    public static function all(): RegistryMap<T> {
        return static::$registered;
    }

    /**
     * Register a file into memory and return an instantiated object.
     *
     * @uses Titon\Utility\Path
     *
     * @param string $key
     * @param array $params
     * @param bool $store
     * @return T
     */
    public static function factory(string $key, ArgumentList $params = Vector {}, bool $store = true): T {
        if (static::has($key)) {
            return static::get($key);
        }

        $namespace = Path::toNamespace($key);
        $reflection = new ReflectionClass($namespace);
        $object = $reflection->newInstanceArgs($params->toArray());

        if ($store) {
            $object = static::set($object, $key);
        }

        return $object;
    }

    /**
     * Flush the registry by removing all stored objects.
     */
    public static function flush(): void {
        static::$registered->clear();
    }

    /**
     * Return the object assigned to the given key.
     *
     * @param string $key
     * @return T
     * @throws \Titon\Utility\Exception\MissingObjectException
     */
    public static function get(string $key): T {
        if (static::has($key)) {
            $object = static::$registered[$key];

            if (is_callable($object)) {
                // UNSAFE
                // Because you can't invariant() a callable
                $object = static::set(call_user_func($object), $key);
            }

            return $object;
        }

        throw new MissingObjectException(sprintf('Object %s does not exist in the registry', $key));
    }

    /**
     * Checks to see if an object has been registered (instantiated).
     *
     * @param string $key
     * @return bool
     */
    public static function has(string $key): bool {
        return static::$registered->contains($key);
    }

    /**
     * Returns an array of all objects that have been registered; returns the keys and not the objects.
     *
     * @return Vector<string>
     */
    public static function keys(): Vector<string> {
        return static::$registered->keys();
    }

    /**
     * Register a callback that will be lazily loaded when called.
     *
     * @param string $key
     * @param \Titon\Utility\RegistryCallback $callback
     */
    public static function register(string $key, RegistryCallback<T> $callback): void {
        // UNSAFE
        // Since the property generics is T while the callback is RegistryCallback<T>
        static::$registered[$key] = $callback;
    }

    /**
     * Remove an object from registry.
     *
     * @param string $key
     */
    public static function remove(string $key): void {
        static::$registered->remove($key);
    }

    /**
     * Store an object into registry.
     *
     * @param T $object
     * @param string $key
     * @return T
     * @throws \Titon\Utility\Exception\InvalidObjectException
     */
    public static function set(T $object, string $key = ''): T {
        if (!is_object($object)) {
            throw new InvalidObjectException('The object to register must be instantiated');
        }

        if (!$key) {
            $key = get_class($object);
        }

        static::$registered[$key] = $object;

        return $object;
    }

}
