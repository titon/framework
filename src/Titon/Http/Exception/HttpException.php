<?hh //strict
/**
 * @copyright   2010-2014, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Http\Exception;

use Titon\Http\Http;
use \Exception;

/**
 * Base Exception for HTTP exceptions.
 *
 * @package Titon\Http\Exception
 */
class HttpException extends Exception {

    /**
     * Set the HTTP status code and message.
     *
     * @param string $message
     * @param int $code
     * @param mixed $previous
     */
    public function __construct($message = null, $code = 0, $previous = null) {
        if (!$message) {
            try {
                $message = Http::getStatusCode($code);
            } catch (Exception $e) {
                $code = 0;
            }
        }

        parent::__construct($message, $code, $previous);
    }

}
