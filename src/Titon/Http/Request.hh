<?hh // strict
/**
 * @copyright   2010-2014, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Http;

/**
 * Represents an HTTP request, either about a server request, or for a client making a request.
 *
 * Based on the PHP-FIG HTTP spec. https://github.com/php-fig/http-message/blob/master/src/RequestInterface.php
 *
 * @package Titon\Http
 */
interface Request extends Message {

    /**
     * Gets the host for the current request.
     *
     * @return string
     */
    public function getHost(): string;

    /**
     * Gets the HTTP method of the request.
     *
     * @return string
     */
    public function getMethod(): string;

    /**
     * Gets the URL protocol scheme for the current request.
     *
     * @return string
     */
    public function getScheme(): string;

    /**
     * Gets the request URL.
     *
     * @return string
     */
    public function getUrl(): string;

    /**
     * Check if the current request is taking place in SSL,
     * or a secure connection.
     *
     * @return bool
     */
    public function isSecure(): bool;

    /**
     * Sets the method to be performed on the resource identified by the
     * Request-URI. While method names are case case-sensitive, implementations
     * SHOULD convert the method to all uppercase characters.
     *
     * @param string $method
     * @return $this
     */
    public function setMethod($method): this;

    /**
     * Sets the request URL.
     *
     * @param string $url
     * @return $this
     */
    public function setUrl($url): this;

}