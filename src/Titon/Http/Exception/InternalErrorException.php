<?hh //strict
/**
 * @copyright   2010-2014, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Http\Exception;

use Titon\Http\Http;

/**
 * Represents an HTTP 500 error.
 *
 * @package Titon\Http\Exception
 */
class InternalErrorException extends HttpException {

    /**
     * {@inheritdoc}
     */
    public function __construct($message = 'Internal Error', $code = Http::INTERNAL_SERVER_ERROR, $previous = null) {
        parent::__construct($message, $code, $previous);
    }

}
