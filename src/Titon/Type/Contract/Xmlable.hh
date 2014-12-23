<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Type\Contract;

/**
 * Interface for enabling to XML support.
 *
 * @package Titon\Type\Contract
 */
interface Xmlable {

    /**
     * Return the object as an XML string.
     *
     * @param string $root
     * @return string
     */
    public function toXml(string $root = 'root'): string;

}