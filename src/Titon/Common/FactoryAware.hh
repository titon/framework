<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Common;

use Titon\Utility\Registry;
use \ReflectionClass;

/**
 * Provides static methods for factoring or registering self instances.
 *
 * @package Titon\Common
 */
trait FactoryAware {

    /**
     * Return a new instance of the current class.
     *
     * @param array<mixed> $args
     * @return $this
     */
    public static function factory<T>(...$args): T {
        return (new ReflectionClass(static::class))->newInstanceArgs($args);
    }

    /**
     * Return a new instance of the current class and store it in registry.
     *
     * @param array<mixed> $args
     * @return $this
     */
    public static function registry<T>(...$args): T {
        return Registry::factory(static::class, $args);
    }

}
