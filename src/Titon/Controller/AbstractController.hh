<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Controller;

use Titon\Common\ArgumentList;
use Titon\Controller\Exception\InvalidActionException;
use Titon\Event\Emittable;
use Titon\Event\Event;
use Titon\Event\Listener;
use Titon\Event\ListenerMap;
use Titon\Event\Subject;
use Titon\Http\Exception\HttpException;
use Titon\Http\Http;
use Titon\Http\IncomingRequestAware;
use Titon\Http\OutgoingResponseAware;
use Titon\Utility\Inflector;
use Titon\Utility\Path;
use Titon\View\View;
use \Exception;

type ActionMap = Map<string, ArgumentList>;

/**
 * The Controller (MVC) acts as the median between the request and response within the dispatch cycle.
 * It splits up its responsibility into multiple Actions, where each Action deals with specific business logic.
 * The logical data is retrieved from a Model (database or logic entity) or a PHP super global (POST, GET).
 *
 * The Controller receives an instance of the View object allowing the Controller to set data to the view,
 * overwrite the View and Engine configuration, attach helpers, etc.
 *
 * @package Titon\Controller
 * @events
 *      controller.processing(Controller $con, string $action, Vector<mixed> $args)
 *      controller.processed(Controller $con, string $action, string $response)
 *      controller.error(Controller $con, Exception $e)
 */
abstract class AbstractController implements Controller, Listener, Subject {
    use Emittable, IncomingRequestAware, OutgoingResponseAware;

    /**
     * The currently dispatched action.
     *
     * @var string
     */
    protected string $_action = 'index';

    /**
     * A mapping of actions that have been dispatched,
     * to a list of arguments used to make the call.
     *
     * @var \Titon\Controller\ActionMap
     */
    protected ActionMap $_arguments = Map {};

    /**
     * View instance.
     *
     * @var \Titon\View\View
     */
    protected ?View $_view;

    /**
     * Initialize events.
     */
    public function __construct() {
        $this->on('controller', $this);
    }

    /**
     * Return a probable path to a view template that matches the current controller and action.
     *
     * @param string $action
     * @return string
     */
    public function buildViewPath(string $action): string {
        $prepare = function(string $path): string {
            return trim(str_replace(['_', 'controller'], ['-', ''], Inflector::underscore($path)), '-');
        };

        return sprintf('%s/%s', $prepare(Path::className(static::class)), $prepare($action));
    }

    /**
     * {@inheritdoc}
     */
    public function dispatchTo(string $action, ArgumentList $args = Vector {}, bool $emit = true): string {
        $this->_action = $action;
        $this->_arguments[$action] = $args;

        // Convert dashed actions to camel case
        if (strpos($action, '-') !== false) {
            $action = lcfirst(Inflector::camelCase($action));
        }

        if ($emit) {
            $this->emit('controller.processing', [$this, $action, $args]);
        }

        // Calling `missingAction()` if the action does not exist
        if (!method_exists($this, $action)) {
            $response = $this->missingAction();

        // Trigger action and generate response from view templates
        } else {

            // UNSAFE
            // Since inst_meth() requires literal strings and we are passing variables
            $response = call_user_func_array(inst_meth($this, $action), $args);
        }

        if ($emit) {
            $this->emit('controller.processed', [$this, $action, &$response]);
        }

        return $response;
    }

    /**
     * {@inheritdoc}
     */
    public function forwardTo(string $action, ArgumentList $args = Vector {}): string {
        return $this->dispatchTo($action, $args, false);
    }

    /**
     * Return the currently defined arguments for a specific action.
     *
     * @param string $action
     * @return \Titon\Common\ArgumentList
     */
    public function getActionArguments(string $action): ArgumentList {
        if ($this->_arguments->contains($action)) {
            return $this->_arguments[$action];
        }

        return Vector {};
    }

    /**
     * Return the name of the currently dispatched action.
     *
     * @return string
     */
    public function getCurrentAction(): string {
        return $this->_action;
    }

    /**
     * Return the arguments for the current action.
     *
     * @return \Titon\Common\ArgumentList
     */
    public function getCurrentArguments(): ArgumentList {
        return $this->getActionArguments($this->getCurrentAction());
    }

    /**
     * {@inheritdoc}
     */
    public function getView(): ?View {
        return $this->_view;
    }

    /**
     * Empty initializer method.
     */
    public function initialize(): void {
        return;
    }

    /**
     * {@inheritdoc}
     *
     * @throws \Titon\Controller\Exception\InvalidActionException
     */
    public function missingAction(): string {
        throw new InvalidActionException(sprintf('Your action %s does not exist. Supply your own missingAction() method to customize this error or view.', $this->getCurrentAction()));
    }

    /**
     * {@inheritdoc}
     */
    public function preProcess(Event $event, Controller $controller, string $action, ArgumentList $args): void {
        return;
    }

    /**
     * {@inheritdoc}
     */
    public function postProcess(Event $event, Controller $controller, string $action, string $response): void {
        return;
    }

    /**
     * Register the events to listen to.
     *
     * @return \Titon\Event\ListenerMap
     */
    public function registerEvents(): ListenerMap {
        return Map {
            'controller.processing' => Map {'method' => 'preProcess', 'priority' => 1},
            'controller.processed' => Map {'method' => 'postProcess', 'priority' => 1}
        };
    }

    /**
     * {@inheritdoc}
     *
     * @uses Titon\Http\Http
     */
    public function renderError(Exception $exception): string {
        $template = (error_reporting() <= 0) ? 'http' : 'error';
        $status = ($exception instanceof HttpException) ? $exception->getCode() : 500;
        $view = $this->getView();

        // Set the response status code
        $this->getResponse()?->statusCode($status);

        // If no view, exit with a generic message
        if (!$view) {
            return 'Internal server error.';
        }

        $this->emit('controller.error', [$this, $exception]);

        return $view
            ->setVariables(Map {
                'pageTitle' => Http::getStatusCode($status),
                'error' => $exception,
                'code' => $status,
                'message' => $exception->getMessage(),
                'url' => $this->getRequest()?->getUrl() ?: ''
            })
            ->render('errors/' . $template, true);
    }

    /**
     * {@inheritdoc}
     */
    public function renderView(): string {
        $view = $this->getView();

        // If no view, return nothing
        if (!$view) {
            return '';
        }

        return $view->render($this->buildViewPath($this->getCurrentAction()));
    }

    /**
     * {@inheritdoc}
     */
    public function runAction(Action $action): string {
        $action->setController($this);

        // UNSAFE
        // Since inst_meth() requires literal strings and we are passing variables
        return call_user_func_array(inst_meth($action, strtolower($this->getRequest()->getMethod())), $this->getCurrentArguments());
    }

    /**
     * {@inheritdoc}
     */
    public function setView(View $view): this {
        $this->_view = $view;

        return $this;
    }

}
