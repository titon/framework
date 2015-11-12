<?hh // partial
// Because of PSR.
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Controller;

use Psr\Http\Message\ResponseInterface;
use Psr\Http\Message\ServerRequestInterface;
use Titon\Controller\Action;
use Titon\View\View;
use \Exception;

/**
 * Interface for the controllers library.
 *
 * @package Titon\Controller
 */
interface Controller {

    /**
     * Dispatch to an action with a list of arguments and use the output as the HTTP response.
     * If an exception is thrown within an action, or an action returns void, automatically render a view.
     *
     * @param string $action
     * @param \Titon\Controller\ArgumentList $args
     * @return \Psr\Http\Message\ResponseInterface
     */
    public function dispatchTo(string $action, ArgumentList $args): ResponseInterface;

    /**
     * Forward the current request to a new action, instead of doing an additional HTTP request.
     *
     * @param string $action
     * @param \Titon\Controller\ArgumentList $args
     * @return \Psr\Http\Message\ResponseInterface
     */
    public function forwardTo(string $action, ArgumentList $args): ResponseInterface;

    /**
     * Return the request object.
     *
     * @return \Psr\Http\Message\ServerRequestInterface
     */
    public function getRequest(): ServerRequestInterface;

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
    public function getView(): ?View;

    /**
     * Method to be called when an action is missing.
     *
     * @return mixed
     */
    public function missingAction(): mixed;

    /**
     * Render a view template for an error/exception and return the output.
     *
     * @param \Exception $exception
     * @return string
     */
    public function renderError(Exception $exception): string;

    /**
     * Render a view template and return the output.
     *
     * @return string
     */
    public function renderView(): string;

    /**
     * Trigger a custom action class that should either return a string or a response object.
     *
     * @param \Titon\Controller\Action $action
     * @return mixed
     */
    public function runAction(Action $action): mixed;

    /**
     * Set the request object.
     *
     * @param \Psr\Http\Message\ServerRequestInterface $request
     * @return $this
     */
    public function setRequest(ServerRequestInterface $request): this;

    /**
     * Set the response object.
     *
     * @param \Psr\Http\Message\ResponseInterface $response
     * @return $this
     */
    public function setResponse(ResponseInterface $response): this;

    /**
     * Set the view object.
     *
     * @param \Titon\View\View $view
     * @return $this
     */
    public function setView(View $view): this;

}
