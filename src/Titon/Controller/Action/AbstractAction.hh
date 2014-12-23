<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Controller\Action;

use Titon\Controller\Action;
use Titon\Controller\Controller;

/**
 * The Action is a sub-routine of the Controller parent and is packaged as a stand-alone object instead of a method.
 * An Action object gives you the flexibility of re-using actions and specific logic across multiple
 * Controllers, encapsulating additional methods within the Action process, and defining its own attachments.
 *
 * @package Titon\Controller\Action
 */
abstract class AbstractAction implements Action {

    /**
     * Controller object.
     *
     * @var \Titon\Controller\Controller
     */
    protected Controller $_controller;

    /**
     * {@inheritdoc}
     */
    public function getController(): Controller {
        return $this->_controller;
    }

    /**
     * {@inheritdoc}
     */
    public function setController(Controller $controller): this {
        $this->_controller = $controller;

        return $this;
    }

}