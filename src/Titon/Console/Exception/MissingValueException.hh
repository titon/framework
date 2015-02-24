<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Console\Exception;

use RuntimeException;

/**
 * Exception thrown when a value is required and not present. This can be the
 * case with options or arguments.
 *
 * @package Titon\Context\Exception
 */
class MissingValueException extends RuntimeException {

}
