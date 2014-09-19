<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
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
     * @return $this
     */
    public static function factory(): this {
        return (new ReflectionClass(static::class))->newInstanceArgs(func_get_args());
    }

    /**
     * Return a new instance of the current class and store it in registry.
     *
     * @return $this
     */
    public static function registry(): this {
        return Registry::factory(static::class, new Vector(func_get_args()));
    }

}