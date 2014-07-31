<?hh // strict
/**
 * @copyright   2010-2014, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Http;

use Psr\Http\Message\ResponseInterface;

/**
 * Permits a class to interact with a response object.
 *
 * @package Titon\Http
 */
trait ResponseAware {

    /**
     * Response object.
     *
     * @type \Psr\Http\Message\ResponseInterface
     */
    protected ?ResponseInterface $_response;

    /**
     * Return the response object.
     *
     * @return \Psr\Http\Message\ResponseInterface
     */
    public function getResponse(): ?ResponseInterface {
        return $this->_response;
    }

    /**
     * Set the response object.
     *
     * @param \Psr\Http\Message\ResponseInterface $response
     * @return $this
     */
    public function setResponse(ResponseInterface $response): this {
        $this->_response = $response;

        return $this;
    }

}