<?hh // strict
/**
 * @copyright   2010-2014, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Http;

use Psr\Http\Message\ResponseInterface;

/**
 * Represents an HTTP response, either for sending a server response, or from a client request.
 *
 * Based on the PHP-FIG HTTP spec. https://github.com/php-fig/http-message/blob/master/src/ResponseInterface.php
 *
 * @package Titon\Http
 */
interface Response extends ResponseInterface {

    /**
     * Pass the current request to the response so that the response
     * can make use of any request data or environment variables.
     *
     * @param \Titon\Http\Request $request
     * @return $this
     */
    public function prepare(Request $request): this;

    /**
     * Output the response by looping through and setting all headers,
     * setting all cookies, and chunking the body response.
     *
     * @return string
     */
    public function send(): string;

}