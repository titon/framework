<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Annotation;

interface Wireable {

    /**
     * When wiring an annotation through the `WiresAnnotations` trait,
     * immediately call this method for any required bootstrapping.
     *
     * The current class being wired as passed as the 1st argument,
     * as well as the method name if a method annotation.
     *
     * @param T $class
     * @param string $method
     * @return $this
     */
    public function wire<T>(T $class, string $method = ''): this;

}
