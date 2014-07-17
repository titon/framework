<?hh // strict
/**
 * @copyright   2010-2014, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Http;

/**
 * Represents common functionality between an HTTP request and response.
 * Based on the PHP-FIG HTTP spec. https://github.com/php-fig/fig-standards/blob/master/proposed/http-message.md
 *
 * @package Titon\Http
 */
interface Message {

    /**
     * Appends a header value to any existing values associated with the
     * given header name.
     *
     * @param string $key Header name to add
     * @param string $value  Value of the header
     * @return $this
     */
    public function addHeader($key, $value);

    /**
     * Merges in an associative array of headers.
     *
     * Each array key MUST be a string representing the case-insensitive name
     * of a header. Each value MUST be either a string or an array of strings.
     * For each value, the value is appended to any existing header of the same
     * name, or, if a header does not already exist by the given name, then the
     * header is added.
     *
     * @param array $headers Associative array of headers to add to the message
     * @return $this
     */
    public function addHeaders(array $headers);

    /**
     * Checks if a header exists by the given case-insensitive name.
     *
     * @param string $key Case-insensitive header name.
     * @return bool Returns true if any header names match the given header
     *     name using a case-insensitive string comparison. Returns false if
     *     no matching header name is found in the message.
     */
    public function hasHeader($key);

    /**
     * Gets the body of the message.
     *
     * @return string
     */
    public function getBody();

    /**
     * Retrieve a header by the given case-insensitive name.
     *
     * By default, this method returns all of the header values of the given
     * case-insensitive header name as a string concatenated together using
     * a comma. Because some header should not be concatenated together using a
     * comma, this method provides a Boolean argument that can be used to
     * retrieve the associated header values as an array of strings.
     *
     * @param string $key  Case-insensitive header name.
     * @param bool   $asArray Set to true to retrieve the header value as an
     *                        array of strings.
     * @return array|string
     */
    public function getHeader($key, $asArray = false);

    /**
     * Gets all message headers.
     *
     * The keys represent the header name as it will be sent over the wire, and
     * each value is an array of strings associated with the header.
     *
     * @return array
     */
    public function getHeaders();

    /**
     * Gets the HTTP protocol version.
     *
     * @return string
     */
    public function getProtocolVersion();

    /**
     * Sets the body of the message.
     *
     * The body MUST be a StreamInterface object. Setting the body to null MUST
     * remove the existing body.
     *
     * @param string $body Body.
     * @return $this
     */
    public function setBody($body = null);

    /**
     * Sets a header, replacing any existing values of any headers with the
     * same case-insensitive name.
     *
     * The header values MUST be a string or an array of strings.
     *
     * @param string       $key Header name
     * @param string|array $value  Header value(s)
     * @return $this
     */
    public function setHeader($key, $value);

    /**
     * Sets headers, replacing any headers that have already been set on the
     * message.
     *
     * The array keys MUST be a string. The array values must be either a
     * string or an array of strings.
     *
     * @param array $headers Headers to set.
     * @return $this
     */
    public function setHeaders(array $headers);

    /**
     * Remove a specific header by case-insensitive name.
     *
     * @param string $key HTTP header to remove
     * @return $this
     */
    public function removeHeader($key);

    /**
     * Remove multiple keys by name.
     *
     * @param array $keys
     * @return $this
     */
    public function removeHeaders(array $keys);

}