<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Http\Exception;

use Titon\Http\Http;
use \Exception;

/**
 * Represents an HTTP 403 error.
 *
 * @package Titon\Http\Exception
 */
class ForbiddenException extends HttpException {

    /**
     * {@inheritdoc}
     */
    public function __construct(string $message = 'Forbidden', int $code = Http::FORBIDDEN, ?Exception $previous = null) {
        parent::__construct($message, $code, $previous);
    }

}
