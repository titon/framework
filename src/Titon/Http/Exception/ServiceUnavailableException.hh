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
 * Represents an HTTP 503 error.
 *
 * @package Titon\Http\Exception
 */
class ServiceUnavailableException extends HttpException {

    /**
     * {@inheritdoc}
     */
    public function __construct(string $message = 'Service Unavailable', int $code = StatusCode::SERVICE_UNAVAILABLE, ?Exception $previous = null) {
        parent::__construct($message, $code, $previous);
    }

}
