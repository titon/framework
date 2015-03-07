<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Controller\Event;

use Titon\Controller\ArgumentList;
use Titon\Controller\Controller;
use Titon\Event\Event;

/**
 * The controller event that occurs before processing an action.
 *
 * @package Titon\Controller\Event
 */
class ProcessingEvent extends Event {

    /**
     * The controller action.
     *
     * @var string
     */
    protected string $action;

    /**
     * The arguments for the action.
     *
     * @var \Titon\Controller\ArgumentList
     */
    protected ArgumentList $args;

    /**
     * The controller instance.
     *
     * @var \Titon\Controller\Controller
     */
    protected Controller $controller;

    /**
     * Store the event settings.
     *
     * @param \Titon\Controller\Controller $controller
     * @param string $action
     * @param \Titon\Controller\ArgumentList $args
     */
    public function __construct(Controller $controller, string $action, ArgumentList $args) {
        $this->controller = $controller;
        $this->action = $action;
        $this->args = $args;

        parent::__construct('controller.processing');
    }

    /**
     * Return the action.
     *
     * @return string
     */
    public function getAction(): string {
        return $this->action;
    }

    /**
     * Return the action arguments.
     *
     * @return \Titon\Controller\ArgumentList
     */
    public function getArguments(): ArgumentList {
        return $this->args;
    }

    /**
     * Return the controller.
     *
     * @return \Titon\Controller\Controller
     */
    public function getController(): Controller {
        return $this->controller;
    }

}
