<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Annotation;

/**
 * The Wireable interface marks an annotation as wireable, which is a form of bootstrapping
 * that occurs when a class wires up annotations within itself. This is handled with
 * the `Titon\Annotation\WiresAnnotations` trait.
 *
 * @package Titon\Annotation
 */
interface Wireable {

    /**
     * When wiring an annotation through the `WiresAnnotations` trait,
     * immediately call this method for any required bootstrapping.
     *
     * The class doing the wiring is passed as the 1st argument,
     * and the method name as the 2nd argument (if a method annotation).
     *
     * @param T $class
     * @param string $method
     * @return $this
     */
    public function wire<T>(T $class, string $method = ''): this;

}
