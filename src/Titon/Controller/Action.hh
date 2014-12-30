<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Controller;

/**
 * Interface for the actions library.
 *
 * @package Titon\Controller
 */
interface Action {

    /**
     * Method that is executed for a DELETE request.
     *
     * @return mixed
     */
    public function delete(): mixed;

    /**
     * Method that is executed for a GET request.
     *
     * @return mixed
     */
    public function get(): mixed;

    /**
     * Return the controller.
     *
     * @return \Titon\Controller\Controller
     */
    public function getController(): Controller;

    /**
     * Set the parent controller.
     *
     * @param \Titon\Controller\Controller $controller
     * @return $this
     */
    public function setController(Controller $controller): this;

    /**
     * Method that is executed for a POST request.
     *
     * @return mixed
     */
    public function post(): mixed;

    /**
     * Method that is executed for a PUT request.
     *
     * @return mixed
     */
    public function put(): mixed;

}