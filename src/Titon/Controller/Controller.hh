<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Controller;

use Titon\Common\ArgumentList;
use Psr\Http\Message\OutgoingResponseInterface;
use Psr\Http\Message\IncomingRequestInterface;
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
     * Dispatch the request to the correct controller action. Checks to see if the action exists and is not protected.
     *
     * @param string $action
     * @param \Titon\Common\ArgumentList $args
     * @param bool $emit
     * @return string
     */
    public function dispatchTo(string $action, ArgumentList $args, bool $emit = true): string;

    /**
     * Forward the current request to a new action, instead of doing an additional HTTP request.
     *
     * @param string $action
     * @param \Titon\Common\ArgumentList $args
     * @return string
     */
    public function forwardTo(string $action, ArgumentList $args): string;

    /**
     * Return the request object.
     *
     * @return \Psr\Http\Message\IncomingRequestInterface
     */
    public function getRequest(): ?IncomingRequestInterface;

    /**
     * Return the response object.
     *
     * @return \Psr\Http\Message\OutgoingResponseInterface
     */
    public function getResponse(): ?OutgoingResponseInterface;

    /**
     * Return the view object.
     *
     * @return \Titon\View\View
     */
    public function getView(): ?View;

    /**
     * Method to be called when an action is missing.
     *
     * @return string
     */
    public function missingAction(): string;

    /**
     * Render the view template for an error/exception.
     * This will automatically set the body of the response.
     *
     * @param \Exception $exception
     * @return string
     */
    public function renderError(Exception $exception): string;

    /**
     * Render the view templates and return the output.
     * This will automatically set the body of the response.
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
     * @param \Psr\Http\Message\IncomingRequestInterface $request
     * @return $this
     */
    public function setRequest(IncomingRequestInterface $request): this;

    /**
     * Set the response object.
     *
     * @param \Psr\Http\Message\OutgoingResponseInterface $response
     * @return $this
     */
    public function setResponse(OutgoingResponseInterface $response): this;

    /**
     * Set the view instance.
     *
     * @param \Titon\View\View $view
     * @return $this
     */
    public function setView(View $view): this;

}
