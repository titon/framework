<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Common;

/**
 * The Stringable trait provides a very basic `__toString()` implementation,
 * which returns the name of the class.
 *
 * @package Titon\Common
 */
trait Stringable {

    /**
     * Return the class name as a string.
     *
     * @return string
     */
    public function toString(): string {
        return static::class;
    }

    /**
     * Alias and magic method for `toString()`.
     *
     * @return string
     */
    public function __toString(): string {
        return $this->toString();
    }

}