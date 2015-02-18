<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Context;

/**
 * The Definition is an interface for depository definitions to follow.
 *
 * @package Titon\Context\Definition
 */
interface Definition {

    /**
     * Arguments passed into the constructor when creating the object from
     * within the container.
     *
     * @param array<mixed> ...$arguments   Arguments passed into the constructor
     *
     * @return $this    The definition for fluent method chaining
     */
    public function with(...$arguments): this;

    /**
     * Use reflection to construct a new object instance of the definition's class
     * given any parameters passed.
     *
     * @param array<mixed> ...$arguments   Arguments passed into the constructor
     *
     * @return $this    The definition for fluent method chaining
     */
    public function create<T>(...$arguments): T;

}
