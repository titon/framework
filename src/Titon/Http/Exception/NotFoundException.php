<?php
/**
 * @copyright   2010-2014, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Http\Exception;

use Titon\Http\Http;

/**
 * Represents an HTTP 404 error.
 *
 * @package Titon\Http\Exception
 */
class NotFoundException extends HttpException {

    /**
     * {@inheritdoc}
     */
    public function __construct($message = 'Not Found', $code = Http::NOT_FOUND, $previous = null) {
        parent::__construct($message, $code, $previous);
    }

}
