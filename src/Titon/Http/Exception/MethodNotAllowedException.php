<?hh // strict
/**
 * @copyright   2010-2014, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Http\Exception;

use Titon\Http\Http;

/**
 * Represents an HTTP 405 error.
 *
 * @package Titon\Http\Exception
 */
class MethodNotAllowedException extends HttpException {

    /**
     * {@inheritdoc}
     */
    public function __construct($message = 'Method Not Allowed', $code = Http::METHOD_NOT_ALLOWED, $previous = null) {
        parent::__construct($message, $code, $previous);
    }

}
