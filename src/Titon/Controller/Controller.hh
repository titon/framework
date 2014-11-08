<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Controller;

use Psr\Http\Message\ResponseInterface;
use Psr\Http\Message\RequestInterface;
use Titon\Controller\Action;
use Titon\View\View;
use \Exception;

type ArgumentList = Vector<mixed>;

/**
 * Interface for the controllers library.
 *
 * @package Titon\Controller
 */
interface Controller {

    /**
     * Dispatch the request to the correct controller action. Checks to see if the action exists and is not protected.
     *
     * @param string $action
     * @param \Titon\Controller\ArgumentList $args
     * @param bool $emit
     * @return string
     */
    public function dispatchTo(string $action, ArgumentList $args = Vector {}, bool $emit = true): string;

    /**
     * Forward the current request to a new action, instead of doing an additional HTTP request.
     *
     * @param string $action
     * @param \Titon\Controller\ArgumentList $args
     * @return string
     */
    public function forwardTo(string $action, ArgumentList $args = Vector {}): string;

    /**
     * Return the request object.
     *
     * @return \Psr\Http\Message\RequestInterface
     */
    public function getRequest(): RequestInterface;

    /**
     * Return the response object.
     *
     * @return \Psr\Http\Message\ResponseInterface
     */
    public function getResponse(): ResponseInterface;

    /**
     * Return the view object.
     *
     * @return \Titon\View\View
     */
    public function getView(): View;

    /**
     * Method to be called when an action is missing.
     *
     * @return string
     */
    public function missingAction(): string;

    /**
     * Render the view template for an error/exception.
     *
     * @param \Exception $exception
     * @return string
     */
    public function renderError(Exception $exception): string;

    /**
     * Render the view templates and return the output.
     *
     * @return string
     */
    public function renderView(): string;

    /**
     * Trigger a custom Action class.
     *
     * @param \Titon\Controller\Action $action
     * @return string
     */
    public function runAction(Action $action): string;

    /**
     * Set the request object.
     *
     * @param \Psr\Http\Message\RequestInterface $request
     * @return $this
     */
    public function setRequest(RequestInterface $request): this;

    /**
     * Set the response object.
     *
     * @param \Psr\Http\Message\ResponseInterface $response
     * @return $this
     */
    public function setResponse(ResponseInterface $response): this;

    /**
     * Set the view instance.
     *
     * @param \Titon\View\View $view
     * @return $this
     */
    public function setView(View $view): this;

}
