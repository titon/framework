<?hh // strict
/**
 * @copyright   2010-2014, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Http\Exception;

use Titon\Http\Http;
use \Exception;

/**
 * Represents an HTTP 400 error.
 *
 * @package Titon\Http\Exception
 */
class BadRequestException extends HttpException {

    /**
     * {@inheritdoc}
     */
    public function __construct(string $message = 'Bad Request', int $code = Http::BAD_REQUEST, ?Exception $previous = null) {
        parent::__construct($message, $code, $previous);
    }

}
