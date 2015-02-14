<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Context\Exception;

use InvalidArgumentException;

/**
 * Exception thrown when a key or alias has already been registered with
 * the depository container.
 *
 * @package Titon\Context\Exception
 */
class AlreadyRegisteredException extends InvalidArgumentException
{
}