<?hh
/**
 * @copyright   2010-2014, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Http\Exception;

use Titon\Http\Http;

/**
 * Represents an HTTP 400 error.
 *
 * @package Titon\Http\Exception
 */
class BadRequestException extends HttpException {

    /**
     * {@inheritdoc}
     */
    public function __construct($message = 'Bad Request', $code = Http::BAD_REQUEST, $previous = null) {
        parent::__construct($message, $code, $previous);
    }

}
