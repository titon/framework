<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Controller;

use Titon\Common\ArgumentList;
use Titon\Controller\Event\ErrorEvent;
use Titon\Controller\Event\ProcessedEvent;
use Titon\Controller\Event\ProcessingEvent;
use Titon\Controller\Exception\InvalidActionException;
use Titon\Event\EmitsEvents;
use Titon\Event\Subject;
use Titon\Http\Exception\HttpException;
use Titon\Http\Http;
use Titon\Http\IncomingRequestAware;
use Titon\Http\OutgoingResponseAware;
use Titon\Http\Stream\MemoryStream;
use Titon\Utility\Inflector;
use Titon\Utility\Path;
use Titon\View\View;
use \Exception;

/**
 * The Controller (MVC) acts as the median between the request and response within the dispatch cycle.
 * It splits up its responsibility into multiple Actions, where each Action deals with specific business logic.
 * The logical data is retrieved from a Model (database or logic entity) or a PHP super global (POST, GET).
 *
 * The Controller receives an instance of the View object allowing the Controller to set data to the view,
 * overwrite the View and Engine configuration, attach helpers, etc.
 *
 * @package Titon\Controller
 */
abstract class AbstractController implements Controller, Subject {
    use EmitsEvents, IncomingRequestAware, OutgoingResponseAware;

    /**
     * The currently dispatched action.
     *
     * @var string
     */
    protected string $action = 'index';

    /**
     * A mapping of actions that have been dispatched,
     * to a list of arguments used to make the call.
     *
     * @var \Titon\Controller\ActionMap
     */
    protected ActionMap $arguments = Map {};

    /**
     * View instance.
     *
     * @var \Titon\View\View
     */
    protected ?View $view;

    /**
     * Return a probable path to a view template that matches the current controller and action.
     *
     * @param string $action
     * @return string
     */
    public function buildViewPath(string $action): string {
        $prepare = ($path) ==> trim(str_replace(['_', 'controller'], ['-', ''], Inflector::underscore($path)), '-');

        return sprintf('%s/%s', $prepare(Path::className(static::class)), $prepare($action));
    }

    /**
     * {@inheritdoc}
     */
    public function dispatchTo(string $action, ArgumentList $args, bool $emit = true): string {
        $this->action = $action;
        $this->arguments[$action] = $args;

        // Convert dashed actions to camel case
        if (strpos($action, '-') !== false) {
            $action = lcfirst(Inflector::camelCase($action));
        }

        // Emit before event
        if ($emit) {
            $this->emit(new ProcessingEvent($this, $action, $args));
        }

        // Calling `missingAction()` if the action does not exist
        if (!method_exists($this, $action)) {
            $response = $this->missingAction();

        // Trigger action and generate response from view templates
        } else {

            // UNSAFE
            // Since inst_meth() requires literal strings and we are passing variables
            $handler = inst_meth($this, $action);
            $response = $handler(...$args);
        }

        // Emit after event
        if ($emit) {
            $event = new ProcessedEvent($this, $action, $this->getResponse());
            $this->emit($event);
            //$response = $event->getResponse();
        }

        return $response;
    }

    /**
     * {@inheritdoc}
     */
    public function forwardTo(string $action, ArgumentList $args): string {
        return $this->dispatchTo($action, $args, false);
    }

    /**
     * Return the currently defined arguments for a specific action.
     *
     * @param string $action
     * @return \Titon\Common\ArgumentList
     */
    public function getActionArguments(string $action): ArgumentList {
        if ($this->arguments->contains($action)) {
            return $this->arguments[$action];
        }

        return [];
    }

    /**
     * Return the name of the currently dispatched action.
     *
     * @return string
     */
    public function getCurrentAction(): string {
        return $this->action;
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
        return $this->view;
    }

    /**
     * {@inheritdoc}
     *
     * @throws \Titon\Controller\Exception\InvalidActionException
     */
    public function missingAction(): string {
        throw new InvalidActionException(sprintf('Your action %s does not exist. Supply your own `missingAction()` method to customize this error or view.', $this->getCurrentAction()));
    }

    /**
     * {@inheritdoc}
     *
     * @uses Titon\Http\Http
     */
    public function renderError(Exception $exception): string {
        $template = (error_reporting() <= 0) ? 'http' : 'error';
        $status = ($exception instanceof HttpException) ? $exception->getCode() : 500;

        // Emit an event
        $this->emit(new ErrorEvent($this, $exception));

        // Render the view
        $output = '';

        if ($view = $this->getView()) {
            $output = $view
                ->setVariables(Map {
                    'pageTitle' => Http::getStatusCode($status),
                    'error' => $exception,
                    'code' => $status,
                    'message' => $exception->getMessage(),
                    'url' => $this->getRequest()?->getUrl() ?: ''
                })
                ->render('errors/' . $template, true);
        }

        if (!$output) {
            $output = 'Internal server error.';
        }

        // Set the response status code and body
        if ($response = $this->getResponse()) {
            $response
                ->setStatus($status)
                ->setBody(new MemoryStream($output));
        }

        return (string) $output;
    }

    /**
     * {@inheritdoc}
     */
    public function renderView(): string {
        $output = $this->getView()?->render($this->buildViewPath($this->getCurrentAction()));

        // Set the response body
        if ($output !== null && ($response = $this->getResponse())) {
            $response->setBody(new MemoryStream($output));
        }

        return (string) $output;
    }

    /**
     * {@inheritdoc}
     */
    public function runAction(Action $action): string {
        $action->setController($this);

        // UNSAFE
        // Since inst_meth() requires literal strings and we are passing variables.
        return call_user_func_array(inst_meth($action, strtolower($this->getRequest()->getMethod())), $this->getCurrentArguments());
    }

    /**
     * {@inheritdoc}
     */
    public function setView(View $view): this {
        $this->view = $view;

        return $this;
    }

}
