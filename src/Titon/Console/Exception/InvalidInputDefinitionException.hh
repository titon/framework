<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Console\Exception;

use RuntimeException;

/**
 * Exception thrown when an `InputDefinition` is requested that hasn't been
 * registered.
 *
 * @package Titon\Console\Exception
 */
class InvalidInputDefinitionException extends RuntimeException {

}