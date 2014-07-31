<?hh // strict
/**
 * @copyright   2010-2014, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Http;

use Psr\Http\Message\RequestInterface;

/**
 * Permits a class to interact with a request object.
 *
 * @package Titon\Http
 */
trait RequestAware {

    /**
     * Request object.
     *
     * @type \Psr\Http\Message\RequestInterface
     */
    protected ?RequestInterface $_request;

    /**
     * Return the request object.
     *
     * @return \Psr\Http\Message\RequestInterface
     */
    public function getRequest(): ?RequestInterface {
        return $this->_request;
    }

    /**
     * Set the request object.
     *
     * @param \Psr\Http\Message\RequestInterface $request
     * @return $this
     */
    public function setRequest(RequestInterface $request): this {
        $this->_request = $request;

        return $this;
    }

}