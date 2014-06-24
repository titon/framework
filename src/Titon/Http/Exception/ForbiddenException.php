<?php
/**
 * @copyright   2010-2014, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Http\Exception;

use Titon\Http\Http;

/**
 * Represents an HTTP 403 error.
 *
 * @package Titon\Http\Exception
 */
class ForbiddenException extends HttpException {

    /**
     * {@inheritdoc}
     */
    public function __construct($message = 'Forbidden', $code = Http::FORBIDDEN, $previous = null) {
        parent::__construct($message, $code, $previous);
    }

}
