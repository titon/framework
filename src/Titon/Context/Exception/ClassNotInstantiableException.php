<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Context\Exception;

use RuntimeException;

/**
 * Exception thrown when attempting to make a class through the depository
 * and the class cannot be instantiated.
 *
 * @package Titon\Context\Exception
 */
class ClassNotInstantiableException extends RuntimeException
{
}