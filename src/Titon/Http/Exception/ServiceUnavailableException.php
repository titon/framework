<?hh //strict
/**
 * @copyright   2010-2014, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Http\Exception;

use Titon\Http\Http;

/**
 * Represents an HTTP 503 error.
 *
 * @package Titon\Http\Exception
 */
class ServiceUnavailableException extends HttpException {

    /**
     * {@inheritdoc}
     */
    public function __construct($message = 'Service Unavailable', $code = Http::SERVICE_UNAVAILABLE, $previous = null) {
        parent::__construct($message, $code, $previous);
    }

}
