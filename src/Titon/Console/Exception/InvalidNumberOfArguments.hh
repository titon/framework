<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Console\Exception;

use RuntimeException;

/**
 * Exception thrown when parameters are passed in the input that do not belong
 * to registered input definitions.
 *
 * @package Titon\Console\Exception
 */
class InvalidNumberOfArgumentsException extends RuntimeException {

}
