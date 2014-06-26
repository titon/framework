<?php
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Common;

use \ReflectionClass;

/**
 * The Instanceable trait provides a low-level multiton interface for classes.
 *
 * @package Titon\Common
 */
trait Instanceable {

    /**
     * Collection of class instances.
     *
     * @type array
     */
    protected static $_instances = array();

    /**
     * Return a count of all active instances.
     *
     * @return int
     */
    public static function countInstances() {
        $class = get_called_class();

        if (empty(static::$_instances[$class])) {
            static::$_instances[$class] = [];
        }

        return count(static::$_instances[$class]);
    }

    /**
     * Flush all instances.
     */
    public static function flushInstances() {
        static::$_instances[get_called_class()] = [];
    }

    /**
     * Return an object instance else instantiate a new one.
     *
     * @param string $key
     * @param array $params
     * @return $this
     */
    public static function getInstance($key = 'default', array $params = []) {
        $class = get_called_class();

        if (isset(static::$_instances[$class][$key])) {
            return static::$_instances[$class][$key];
        }

        $reflection = new ReflectionClass($class);
        $object = $reflection->newInstanceArgs($params);

        static::$_instances[$class][$key] = $object;

        return $object;
    }

    /**
     * Remove an instance.
     *
     * @param string $key
     */
    public static function removeInstance($key = 'default') {
        $class = get_called_class();

        unset(static::$_instances[$class][$key]);
    }

    /**
     * Disable cloning.
     *
     * @codeCoverageIgnore
     */
    private function __clone() {}

}