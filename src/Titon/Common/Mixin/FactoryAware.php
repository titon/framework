<?php
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Common\Mixin;

use Titon\Common\Registry;
use \ReflectionClass;

/**
 * Provides static methods for factoring or registering self instances.
 *
 * @package Titon\Common\Mixin
 */
trait FactoryAware {

    /**
     * Return a new instance of the current class.
     *
     * @return $this
     */
    public static function factory() {
        return (new ReflectionClass(get_called_class()))->newInstanceArgs(func_get_args());
    }

    /**
     * Return a new instance of the current class and store it in registry.
     *
     * @return $this
     */
    public static function registry() {
        return Registry::factory(get_called_class(), func_get_args());
    }

}