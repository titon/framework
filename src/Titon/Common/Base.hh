<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Common;

use Titon\Common\Reflectable;

/**
 * Primary class for all framework classes to extend.
 *
 * @package Titon\Common
 */
class Base {
    use Reflectable;

    /**
     * Magic method for toString().
     *
     * @return string
     */
    public function __toString(): string {
        return $this->toString();
    }

    /**
     * Primary initialize method that is triggered during instantiation.
     */
    public function initialize(): void {
        return;
    }

    /**
     * A dummy function for no operation.
     */
    public function noop(): void {
        return;
    }

    /**
     * Return the class name when called as a string.
     *
     * @return string
     */
    public function toString(): string {
        return static::class;
    }

}