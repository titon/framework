<?hh // strict
/**
 * @copyright   2010-2014, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Http;

/**
 * Permits a class to interact with a request object.
 *
 * @package Titon\Http
 */
trait RequestAware {

    /**
     * Request object.
     *
     * @type \Titon\Http\Request
     */
    protected ?Request $_request;

    /**
     * Return the request object.
     *
     * @return \Titon\Http\Request
     */
    public function getRequest(): ?Request {
        return $this->_request;
    }

    /**
     * Set the request object.
     *
     * @param \Titon\Http\Request $request
     * @return $this
     */
    public function setRequest(Request $request): this {
        $this->_request = $request;

        return $this;
    }

}