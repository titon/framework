<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Type\Contract;

/**
 * Interface for enabling to JSON support.
 *
 * @package Titon\Type\Contract
 */
interface Jsonable {

    /**
     * Return the object as a JSON string.
     *
     * @param int $options
     * @return string
     */
    public function toJson(int $options = 0): string;

}