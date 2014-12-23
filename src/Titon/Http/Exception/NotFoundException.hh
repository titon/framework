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
 * Represents an HTTP 404 error.
 *
 * @package Titon\Http\Exception
 */
class NotFoundException extends HttpException {

    /**
     * {@inheritdoc}
     */
    public function __construct(string $message = 'Not Found', int $code = Http::NOT_FOUND, ?Exception $previous = null) {
        parent::__construct($message, $code, $previous);
    }

}
