<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Console\Exception;

use OutOfBoundsException;

/**
 * Exception thrown when the command used in the application doesn't exist.
 *
 * @package Titon\Console\Exception
 */
class InvalidCommandException extends OutOfBoundsException {

}