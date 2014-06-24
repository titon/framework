<?php
/**
 * @copyright   2010-2014, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Http;

/**
 * Represents an HTTP request, either about a server request, or for a client making a request.
 * Based on the PHP-FIG HTTP spec. https://github.com/php-fig/fig-standards/blob/master/proposed/http-message.md
 *
 * @package Titon\Http
 */
interface Request extends Message {

    /**
     * Gets the host for the current request.
     *
     * @return string
     */
    public function getHost();

    /**
     * Gets the HTTP method of the request.
     *
     * @return string
     */
    public function getMethod();

    /**
     * Gets the URL protocol scheme for the current request.
     *
     * @return string
     */
    public function getScheme();

    /**
     * Gets the request URL.
     *
     * @return string
     */
    public function getUrl();

    /**
     * Check if the current request is taking place in SSL,
     * or a secure connection.
     *
     * @return bool
     */
    public function isSecure();

    /**
     * Sets the method to be performed on the resource identified by the
     * Request-URI. While method names are case case-sensitive, implementations
     * SHOULD convert the method to all uppercase characters.
     *
     * @param string $method
     * @return $this
     */
    public function setMethod($method);

    /**
     * Sets the request URL.
     *
     * @param string $url
     * @return $this
     */
    public function setUrl($url);

}