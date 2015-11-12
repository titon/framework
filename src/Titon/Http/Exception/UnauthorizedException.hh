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
 * Represents an HTTP 401 error.
 *
 * @package Titon\Http\Exception
 */
class UnauthorizedException extends HttpException {

    /**
     * {@inheritdoc}
     */
    public function __construct(string $message = 'Unauthorized', int $code = StatusCode::UNAUTHORIZED, ?Exception $previous = null) {
        parent::__construct($message, $code, $previous);
    }

}
