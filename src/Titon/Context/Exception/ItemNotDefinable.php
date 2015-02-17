<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Context\Exception;

use RuntimeException;

/**
 * Exception thrown when a definition cannot be created from the item passed into
 * the depository as a concrete
 *
 * @package Titon\Context\Exception
 */
class ItemNotDefinable extends RuntimeException{

}