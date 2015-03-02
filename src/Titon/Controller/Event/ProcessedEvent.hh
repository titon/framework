<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Controller\Event;

use Psr\Http\Message\OutgoingResponseInterface;
use Titon\Controller\Controller;
use Titon\Event\Event;

/**
 * The controller event that occurs after processing an action.
 * The response can be modified from this event.
 *
 * @package Titon\Controller\Event
 */
class ProcessedEvent extends Event {

    /**
     * The controller action.
     *
     * @var string
     */
    protected string $action;

    /**
     * The controller instance.
     *
     * @var \Titon\Controller\Controller
     */
    protected Controller $controller;

    /**
     * The response instance.
     *
     * @var \Psr\Http\Message\OutgoingResponseInterface
     */
    protected OutgoingResponseInterface $response;

    /**
     * Store the event settings.
     *
     * @param \Titon\Controller\Controller $controller
     * @param string $action
     * @param \Psr\Http\Message\OutgoingResponseInterface $response
     */
    public function __construct(Controller $controller, string $action, OutgoingResponseInterface $response) {
        $this->controller = $controller;
        $this->action = $action;
        $this->response = $response;

        parent::__construct('controller.processed');
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
     * Return the controller.
     *
     * @return \Titon\Controller\Controller
     */
    public function getController(): Controller {
        return $this->controller;
    }

    /**
     * Return the response.
     *
     * @return \Psr\Http\Message\OutgoingResponseInterface
     */
    public function getResponse(): OutgoingResponseInterface {
        return $this->response;
    }

}
