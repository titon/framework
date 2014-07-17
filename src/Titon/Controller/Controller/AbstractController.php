<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Controller\Controller;

use Titon\Common\Base;
use Titon\Common\Attachable;
use Titon\Controller\Action;
use Titon\Controller\Controller;
use Titon\Controller\Exception\InvalidActionException;
use Titon\Event\Emittable;
use Titon\Event\Event;
use Titon\Event\Listener;
use Titon\Http\Exception\HttpException;
use Titon\Http\Http;
use Titon\Http\RequestAware;
use Titon\Http\ResponseAware;
use Titon\Mvc\Module;
use Titon\Utility\Hash;
use Titon\Utility\Inflector;
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
 * Furthermore, the Controller inherits all functionality from the Attachable class, allowing you to attach
 * external classes to use their functionality and trigger specific callbacks.
 *
 * @package Titon\Controller\Controller
 * @events
 *      controller.preProcess(Controller $con, $action, $args)
 *      controller.postProcess(Controller $con, $action, $response)
 *      controller.onError(Controller $con, Exception $exc)
 */
abstract class AbstractController extends Base implements Controller, Listener {
    use Attachable, Emittable, RequestAware, ResponseAware;

    /**
     * Configuration.
     *
     * @type array {
     *      @type string $module        Current application module
     *      @type string $controller    Current controller within the module
     *      @type string $action        Current action within the controller
     *      @type string $ext           The extension within the address bar, and what content-type to render the page as
     *      @type array $args           Action arguments
     *      @type string $template      The custom view template to render
     *      @type bool $render          To render the view or not
     * }
     */
    protected $_config = [
        'module' => '',
        'controller' => '',
        'action' => '',
        'ext' => '',
        'args' => [],
        'template' => '',
        'render' => true,

         // Don't initialize immediately since we need to bootstrap it
        'initialize' => false
    ];

    /**
     * View instance.
     *
     * @type \Titon\View\View
     */
    protected $_view;

    /**
     * Initialize class and events.
     *
     * @param array $config
     */
    public function __construct(array $config = []) {
        parent::__construct($config);

        $this->on('controller', $this);
    }

    /**
     * {@inheritdoc}
     *
     * @throws \Titon\Controller\Exception\InvalidActionException
     */
    public function dispatchAction($action = null, array $args = [], $emit = true) {
        if (!$action) {
            $action = $this->getConfig('action');
        }

        if (!$args) {
            $args = $this->getConfig('args');
        }

        // Convert dashed actions to camel case
        if (mb_strpos($action, '-') !== false) {
            $action = lcfirst(Inflector::camelCase($action));
        }

        if ($emit) {
            $this->emit('controller.preProcess', [$this, &$action, &$args]);
        }

        // Do not include the base controller methods
        $methods = array_diff(get_class_methods($this), get_class_methods(__CLASS__));

        if (!in_array($action, $methods)) {
            $response = $this->missingAction();

        // Trigger action and generate response from view templates
        } else {
            $response = call_user_func_array([$this, $action], $args);
        }

        if ($emit) {
            $this->emit('controller.postProcess', [$this, $action, &$response]);
        }

        return $response;
    }

    /**
     * {@inheritdoc}
     */
    public function forwardAction($action, array $args = []) {
        return $this->setConfig('action', $action)->dispatchAction($action, $args, false);
    }

    /**
     * {@inheritdoc}
     */
    public function getView() {
        return $this->_view;
    }

    /**
     * {@inheritdoc}
     */
    public function missingAction() {
        throw new InvalidActionException(sprintf('Your action %s does not exist, or is not public, or is found within the parent controller.
            Supply your own missingAction() method to customize this error.', $this->getConfig('action')));
    }

    /**
     * {@inheritdoc}
     */
    public function preProcess(Event $event, Controller $controller, &$action, array &$args) {
        $this->notifyObjects('preProcess', [$controller, &$action, &$args]);
    }

    /**
     * {@inheritdoc}
     */
    public function postProcess(Event $event, Controller $controller, $action, &$response) {
        $this->notifyObjects('postProcess', [$controller, $action, &$response]);
    }

    /**
     * Register the events to listen to.
     *
     * @return array
     */
    public function registerEvents() {
        return [
            'controller.preProcess' => ['method' => 'preProcess', 'priority' => 1],
            'controller.postProcess' => ['method' => 'postProcess', 'priority' => 1]
        ];
    }

    /**
     * {@inheritdoc}
     *
     * @uses Titon\Http\Http
     */
    public function renderError(Exception $exception) {
        $template = 'error';
        $status = 500;

        if (error_reporting() <= 0) {
            $template = 'http';
        }

        if ($exception instanceof HttpException) {
            $status = $exception->getCode();
        }

        $this->emit('controller.onError', [$this, $exception]);

        $this->getResponse()->setStatusCode($status);

        return $this->getView()
            ->setVariables([
                'pageTitle' => Http::getStatusCode($status),
                'error' => $exception,
                'code' => $status,
                'message' => $exception->getMessage(),
                'url' => $this->getRequest()->getUrl()
            ])
            ->render(['errors', $template], true);
    }

    /**
     * {@inheritdoc}
     */
    public function renderView($template = null) {
        $config = $this->allConfig();

        if (!$config['render']) {
            return null;
        }

        if (!$template) {
            if (!empty($config['template'])) {
                $template = $config['template'];
            } else {
                $template = Hash::reduce($config, ['controller', 'action', 'ext', 'locale']);
            }
        }

        return $this->getView()->render($template);
    }

    /**
     * {@inheritdoc}
     */
    public function runAction(Action $action) {
        $action->setController($this);

        return call_user_func_array([$action, $this->getRequest()->getMethod()], $this->getConfig('args'));
    }

    /**
     * {@inheritdoc}
     */
    public function setView(View $view) {
        $this->_view = $view;

        return $view;
    }

}
