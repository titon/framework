<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Common;

/**
 * Interface for enabling to array support.
 *
 * @package Titon\Common
 */
interface Arrayable<Tk, Tv> {

    /**
     * Return the object as an array.
     *
     * @return array<Tk, Tv>
     */
    public function toArray(): array<Tk, Tv>;

}