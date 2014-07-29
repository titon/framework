<?hh // strict
/**
 * @copyright   2010-2014, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Http;

/**
 * Permits a class to interact with a response object.
 *
 * @package Titon\Http
 */
trait ResponseAware {

    /**
     * Response object.
     *
     * @type \Titon\Http\Response
     */
    protected ?Response $_response;

    /**
     * Return the response object.
     *
     * @return \Titon\Http\Response
     */
    public function getResponse(): ?Response {
        return $this->_response;
    }

    /**
     * Set the response object.
     *
     * @param \Titon\Http\Response $response
     * @return $this
     */
    public function setResponse(Response $response): this {
        $this->_response = $response;

        return $this;
    }

}