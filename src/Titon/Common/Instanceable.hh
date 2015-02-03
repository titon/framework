<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
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
trait Instanceable<T> {

    /**
     * Collection of class instances.
     *
     * @var \Titon\Common\InstanceContainer
     */
    protected static InstanceContainer<T> $instances = Map {};

    /**
     * Return a count of all active instances.
     *
     * @return int
     */
    public static function countInstances(): int {
        return static::getInstances()->count();
    }

    /**
     * Flush all instances.
     */
    public static function flushInstances(): void {
        static::getInstances()->clear();
    }

    /**
     * Return an object instance else instantiate a new one.
     *
     * @param string $key
     * @param array<mixed> $args
     * @return $this
     */
    public static function getInstance(string $key = 'default', ...$args): T {
        $instances = static::getInstances();

        if ($instances->contains($key)) {
            return $instances[$key];
        }

        return $instances[$key] = (new ReflectionClass(static::class))->newInstanceArgs($args);
    }

    /**
     * Return all instances for a class.
     *
     * @return \Titon\Common\InstanceMap
     */
    public static function getInstances(): InstanceMap<T> {
        $instances = static::$instances;
        $class = static::class;

        if (!$instances->contains($class)) {
            $instances[$class] = Map {};
        }

        return $instances[$class];
    }

    /**
     * Remove an instance.
     *
     * @param string $key
     */
    public static function removeInstance(string $key = 'default'): void {
        static::getInstances()->remove($key);
    }

    /**
     * Disable cloning.
     */
    private function __clone(): void {}

}
