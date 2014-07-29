<?hh // strict
/**
 * @copyright   2010-2014, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Http;

/**
 * Represents an HTTP response, either for sending a server response, or from a client request.
 *
 * Based on the PHP-FIG HTTP spec. https://github.com/php-fig/http-message/blob/master/src/ResponseInterface.php
 *
 * @package Titon\Http
 */
interface Response extends Message {

    /**
     * Gets the response Reason-Phrase, a short textual description of the
     * Status-Code.
     *
     * Because a Reason-Phrase is not a required element in response
     * Status-Line, the Reason-Phrase value MAY be null. Implementations MAY
     * choose to return the default RFC 2616 recommended reason phrase for the
     * response's Status-Code.
     *
     * @return string
     */
    public function getReasonPhrase(): string;

    /**
     * Gets the response Status-Code, a 3-digit integer result code of the
     * server's attempt to understand and satisfy the request.
     *
     * @return integer Status code.
     */
    public function getStatusCode(): int;

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

    /**
     * Sets the Reason-Phrase of the response.
     *
     * If no Reason-Phrase is specified, implementations MAY choose to default
     * to the RFC 2616 recommended reason phrase for the response's Status-Code.
     *
     * @param string $phrase The Reason-Phrase to set.
     * @return $this
     */
    public function setReasonPhrase($phrase): this;

    /**
     * Set the status code to use in the response.
     *
     * @param int $code
     * @return $this
     */
    public function setStatusCode($code): this;

}