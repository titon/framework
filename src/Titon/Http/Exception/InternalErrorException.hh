<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Http\Exception;

use Titon\Http\StatusCode;
use \Exception;

/**
 * Represents an HTTP 500 error.
 *
 * @package Titon\Http\Exception
 */
class InternalErrorException extends HttpException {

    /**
     * {@inheritdoc}
     */
    public function __construct(string $message = 'Internal Error', int $code = StatusCode::INTERNAL_SERVER_ERROR, ?Exception $previous = null) {
        parent::__construct($message, $code, $previous);
    }

}
