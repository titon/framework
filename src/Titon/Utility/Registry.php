<?php
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Utility;

use Titon\Common\Exception\InvalidObjectException;
use Titon\Common\Exception\MissingObjectException;
use \Closure;
use \ReflectionClass;

/**
 * The Registry acts a central hub where any part of the application can access a single instance of a stored object.
 * It registers all objects into the class to store in memory and be re-usable later at any given time.
 *
 * @package Titon\Utility
 */
class Registry {

    /**
     * Objects that have been registered into memory. The array index is represented by the namespace convention,
     * where as the array value would be the matching instantiated object.
     *
     * @type object[]
     */
    protected static $_registered = [];

    /**
     * Return all registered objects.
     *
     * @return object[]
     */
    public static function all() {
        return static::$_registered;
    }

    /**
     * Register a file into memory and return an instantiated object.
     *
     * @uses Titon\Utility\Path
     *
     * @param string $key
     * @param array $params
     * @param bool $store
     * @return object
     */
    public static function &factory($key, array $params = [], $store = true) {
        if (static::has($key)) {
            return static::get($key);
        }

        $namespace = Path::toNamespace($key);
        $reflection = new ReflectionClass($namespace);
        $object = $reflection->newInstanceArgs($params);

        if ($store) {
            $object = static::set($object, $key);
        }

        return $object;
    }

    /**
     * Flush the registry by removing all stored objects.
     */
    public static function flush() {
        static::$_registered = [];
    }

    /**
     * Return the object assigned to the given key.
     *
     * @param string $key
     * @return object
     * @throws \Titon\Common\Exception\MissingObjectException
     */
    public static function &get($key) {
        if (static::has($key)) {
            $object = static::$_registered[$key];

            if ($object instanceof Closure) {
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
    public static function has($key) {
        return isset(static::$_registered[$key]);
    }

    /**
     * Returns an array of all objects that have been registered; returns the keys and not the objects.
     *
     * @return string[]
     */
    public static function keys() {
        return array_keys(static::$_registered);
    }

    /**
     * Register a callback that will be lazily loaded when called.
     *
     * @param string $key
     * @param \Closure $callback
     */
    public static function register($key, Closure $callback) {
        static::set($callback, $key);
    }

    /**
     * Remove an object from registry.
     *
     * @param string $key
     */
    public static function remove($key) {
        unset(static::$_registered[$key]);
    }

    /**
     * Store an object into registry.
     *
     * @param object|\Closure $object
     * @param string $key
     * @return object
     * @throws \Titon\Common\Exception\InvalidObjectException
     */
    public static function set($object, $key = null) {
        if (!is_object($object)) {
            throw new InvalidObjectException('The object to register must be instantiated');
        }

        if (!$key) {
            $key = get_class($object);
        }

        static::$_registered[$key] = $object;

        return $object;
    }

}
