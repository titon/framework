<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Controller\Event;

use Titon\Controller\Controller;
use Titon\Event\Event;
use Exception;

/**
 * The controller event that occurs when an exception or error is thrown.
 *
 * @package Titon\Controller\Event
 */
class ErrorEvent extends Event {

    /**
     * The controller instance.
     *
     * @var \Titon\Controller\Controller
     */
    protected Controller $controller;

    /**
     * The thrown exception.
     *
     * @var \Exception
     */
    protected Exception $exception;

    /**
     * Store the event settings.
     *
     * @param \Titon\Controller\Controller $controller
     * @param \Exception $exception
     */
    public function __construct(Controller $controller, Exception $exception) {
        $this->controller = $controller;
        $this->exception = $exception;

        parent::__construct('controller.error');
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
     * Return the exception.
     *
     * @return \Exception
     */
    public function getException(): Exception {
        return $this->exception;
    }

}
