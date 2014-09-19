<?hh // strict
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
     * @type Map<string, Map<string, mixed>>
     */
    protected static Map<string, Map<string, mixed>> $_instances = Map {};

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
     * @param array $params
     * @return $this
     */
    public static function getInstance(string $key = 'default', array<mixed> $params = []): mixed {
        $instances = static::getInstances();

        if ($instances->contains($key)) {
            return $instances->get($key);
        }

        $reflection = new ReflectionClass(static::class);
        $object = $reflection->newInstanceArgs($params);

        $instances->set($key, $object);

        return $object;
    }

    /**
     * Return all instances for a class.
     *
     * @return Map<string, mixed>
     */
    public static function getInstances(): Map<string, mixed> {
        $instances = static::$_instances;
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
     *
     * @codeCoverageIgnore
     */
    private function __clone(): void {}

}