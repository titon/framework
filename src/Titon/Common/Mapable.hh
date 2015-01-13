<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Common;

/**
 * Interface for enabling to map support.
 *
 * @package Titon\Common
 */
interface Mapable<Tk, Tv> {

    /**
     * Return the object as a map.
     *
     * @return Map<Tk, Tv>
     */
    public function toMap(): Map<Tk, Tv>;

}