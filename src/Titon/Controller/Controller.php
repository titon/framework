<?hh
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Controller;

use Titon\Controller\Action;
use Titon\Event\Event;
use Titon\Http\Request;
use Titon\Http\Response;
use Titon\Mvc\Module;
use Titon\View\View;

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
     * @param array $args
     * @param bool $emit
     * @return string
     */
    public function dispatchAction($action = null, array $args = [], $emit = true);

    /**
     * Forward the current request to a new action, instead of doing an additional HTTP request.
     *
     * @param string $action
     * @param array $args
     * @return string
     */
    public function forwardAction($action, array $args = []);

    /**
     * Return the request object.
     *
     * @return \Titon\Http\Request
     */
    public function getRequest();

    /**
     * Return the response object.
     *
     * @return \Titon\Http\Response
     */
    public function getResponse();

    /**
     * Return the view object.
     *
     * @return \Titon\View\View
     */
    public function getView();

    /**
     * Method to be called when an action is missing.
     *
     * @return string
     */
    public function missingAction();

    /**
     * Triggered before the controller processes the requested action.
     *
     * @param \Titon\Event\Event $event
     * @param \Titon\Controller\Controller $controller
     * @param string $action
     * @param array $args
     */
    public function preProcess(Event $event, Controller $controller, &$action, array &$args);

    /**
     * Triggered after the action processes, but before the view renders.
     *
     * @param \Titon\Event\Event $event
     * @param \Titon\Controller\Controller $controller
     * @param string $action
     * @param string $response
     */
    public function postProcess(Event $event, Controller $controller, $action, &$response);

    /**
     * Render the view template for an error/exception.
     *
     * @param \Exception $exception
     * @return string
     */
    public function renderError(\Exception $exception);

    /**
     * Render the view templates and return the output.
     *
     * @param string|array $template
     * @return string
     */
    public function renderView($template = null);

    /**
     * Trigger a custom Action class.
     *
     * @param \Titon\Controller\Action $action
     * @return string
     */
    public function runAction(Action $action);

    /**
     * Set the request object.
     *
     * @param \Titon\Http\Request $request
     * @return $this
     */
    public function setRequest(Request $request);

    /**
     * Set the response object.
     *
     * @param \Titon\Http\Response $response
     * @return $this
     */
    public function setResponse(Response $response);

    /**
     * Set the view instance.
     *
     * @param \Titon\View\View $view
     * @return \Titon\View\View
     */
    public function setView(View $view);

}
