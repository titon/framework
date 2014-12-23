<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Http;

use Psr\Http\Message\OutgoingResponseInterface;

/**
 * Represents an outgoing server HTTP response.
 *
 * @package Titon\Http
 */
interface OutgoingResponse extends OutgoingResponseInterface {

    /**
     * Pass the current request to the response so that the response
     * can make use of any request data or environment variables.
     *
     * @param \Titon\Http\IncomingRequest $request
     * @return $this
     */
    public function prepare(IncomingRequest $request): this;

    /**
     * Output the response by looping through and setting all headers,
     * setting all cookies, and chunking the body response.
     *
     * @return string
     */
    public function send(): string;

}