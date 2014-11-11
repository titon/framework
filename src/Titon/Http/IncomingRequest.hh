<?hh // strict
/**
 * @copyright   2010-2014, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Http;

use Psr\Http\Message\IncomingRequestInterface;

/**
 * Represents an incoming server HTTP request.
 *
 * @package Titon\Http
 */
interface IncomingRequest extends IncomingRequestInterface {

    /**
     * Gets the host for the current request.
     *
     * @return string
     */
    public function getHost(): string;

    /**
     * Gets the URL protocol scheme for the current request.
     *
     * @return string
     */
    public function getScheme(): string;

    /**
     * Check if the current request is taking place in SSL,
     * or a secure connection.
     *
     * @return bool
     */
    public function isSecure(): bool;

}