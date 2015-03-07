<?hh // partial
// Because of PSR HTTP Message
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Controller;

use Psr\Http\Message\IncomingRequestInterface;
use Psr\Http\Message\OutgoingResponseInterface;
use Psr\Http\Message\StreamableInterface;
use Titon\Controller\Event\ErrorEvent;
use Titon\Controller\Event\ProcessedEvent;
use Titon\Controller\Event\ProcessingEvent;
use Titon\Event\EmitsEvents;
use Titon\Event\Subject;
use Titon\Http\Exception\HttpException;
use Titon\Http\Exception\NotFoundException;
use Titon\Http\Http;
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
    use EmitsEvents;

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
     * Request object.
     *
     * @var \Psr\Http\Message\IncomingRequestInterface
     */
    protected IncomingRequestInterface $request;

    /**
     * Response object.
     *
     * @var \Psr\Http\Message\OutgoingResponseInterface
     */
    protected OutgoingResponseInterface $response;

    /**
     * View instance.
     *
     * @var \Titon\View\View
     */
    protected ?View $view;

    /**
     * Store the request and response.
     *
     * @param \Psr\Http\Message\IncomingRequestInterface $request
     * @param \Psr\Http\Message\OutgoingResponseInterface $response
     */
    public function __construct(IncomingRequestInterface $request, OutgoingResponseInterface $response) {
        $this->request = $request;
        $this->response = $response;
    }

    /**
     * {@inheritdoc}
     */
    public function dispatchTo(string $action, ArgumentList $args, bool $emit = true): OutgoingResponseInterface {
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

        // Attempt to dispatch to an action
        try {

            // Call `missingAction()` if the action does not exist
            if (!method_exists($this, $action)) {
                $response = $this->missingAction();

            // Trigger action and generate response
            } else {
                // UNSAFE
                // Since `inst_meth()` requires literal strings and we are passing variables
                $handler = inst_meth($this, $action);
                $response = $handler(...$args);
            }

            // If response is empty, render a view
            if ($response === null) {
                $response = $this->renderView();
            }

        // If an action throws an exception, render an error
        } catch (HttpException $e) {
            $response = $this->renderError($e);
        }

        // Handle the response
        $response = $this->handleResponse($response);

        // Emit after event
        if ($emit) {
            $event = new ProcessedEvent($this, $action, $response);
            $this->emit($event);
            $response = $event->getResponse();
        }

        return $response;
    }

    /**
     * {@inheritdoc}
     */
    public function forwardTo(string $action, ArgumentList $args): OutgoingResponseInterface {
        return $this->dispatchTo($action, $args, false);
    }

    /**
     * Return the currently defined arguments for a specific action.
     *
     * @param string $action
     * @return \Titon\Controller\ArgumentList
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
     * @return \Titon\Controller\ArgumentList
     */
    public function getCurrentArguments(): ArgumentList {
        return $this->getActionArguments($this->getCurrentAction());
    }

    /**
     * {@inheritdoc}
     */
    public function getRequest(): IncomingRequestInterface {
        return $this->request;
    }

    /**
     * {@inheritdoc}
     */
    public function getResponse(): OutgoingResponseInterface {
        return $this->response;
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
    public function missingAction(): mixed {
        throw new NotFoundException(sprintf('Your action %s does not exist. Supply your own `missingAction()` method to customize this error or view.', $this->getCurrentAction()));
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
        $response = 'Internal server error.';

        if ($view = $this->getView()) {
            $response = $view
                ->setVariables(Map {
                    'pageTitle' => Http::getStatusCode($status),
                    'error' => $exception,
                    'code' => $status,
                    'message' => $exception->getMessage(),
                    'url' => $this->getRequest()->getUrl()
                })
                ->render('errors/' . $template, true);
        }

        // Set the response status code
        $this->getResponse()->setStatus($status);

        return $response;
    }

    /**
     * {@inheritdoc}
     */
    public function renderView(): string {
        return (string) $this->getView()?->render($this->buildViewPath($this->getCurrentAction()));
    }

    /**
     * {@inheritdoc}
     */
    public function runAction(Action $action): mixed {
        $action->setController($this);

        // UNSAFE
        // Since inst_meth() requires literal strings and we are passing variables.
        $callback = inst_meth($action, strtolower($this->getRequest()->getMethod()));
        $arguments = $this->getCurrentArguments();

        return $callback(...$arguments);
    }

    /**
     * {@inheritdoc}
     */
    public function setRequest(IncomingRequestInterface $request): this {
        $this->request = $request;

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function setResponse(OutgoingResponseInterface $response): this {
        $this->response = $response;

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function setView(View $view): this {
        $this->view = $view;

        return $this;
    }

    /**
     * Return a probable path to a view template that matches the current controller and action.
     *
     * @param string $action
     * @return string
     */
    protected function buildViewPath(string $action): string {
        $prepare = ($path) ==> trim(str_replace(['_', 'controller'], ['-', ''], Inflector::underscore($path)), '-');

        return sprintf('%s/%s', $prepare(Path::className(static::class)), $prepare($action));
    }

    /**
     * Handle the response of an action by either setting the body on the response object,
     * or setting a new response.
     *
     * @param mixed $output
     * @return \Psr\Http\Message\OutgoingResponseInterface
     */
    protected function handleResponse(mixed $output): OutgoingResponseInterface {
        $response = $this->getResponse();

        // If the return of an action is a response object
        // We should overwrite the original one and return the new one
        if ($output instanceof OutgoingResponseInterface) {
            $this->setResponse($output);

            return $output;
        }

        // If the return of an action is a stream object
        // Set the body of the response directly
        if ($output instanceof StreamableInterface) {
            $response->setBody($output);

            return $response;
        }

        // Else the output is a string (or should be cast to one)
        // So set the body on the current response and return it
        $response->setBody(new MemoryStream((string) $output));

        return $response;
    }

}
