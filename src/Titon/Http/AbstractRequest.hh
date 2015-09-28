<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Http;

use Psr\Http\Message\RequestInterface;

/**
 * Provides shared functionality for request classes.
 *
 * @package Titon\Http
 */
abstract class AbstractRequest extends AbstractMessage implements RequestInterface {

}
