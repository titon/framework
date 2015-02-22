<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Context;

/**
 * The ServiceProvider is an interface that all Service Providers should follow.
 *
 * @package Titon\Context
 */
interface ServiceProvider {

    /**
     * If the Service Provider has not already been initialized, this method will
     * call the `register`. This is used instead of calling `register` directly
     * to prevent the multiple registration of the same items.
     *
     * @return void
     */
    public function initialize(): void;

    /**
     * Return the Vector collection of class names that this Service Provider
     * provides.
     *
     * @return \Titon\Context\ClassList
     */
    public function getProvides(): ClassList;

    /**
     * This method determines if this Service Provider has the given class name
     * in its `provides` Vector.
     *
     * @param string $value The class name to check
     *
     * @return bool
     */
    public function provides(string $class): bool;

    /**
     * The `register` method is where any necessary retrieving and registering
     * with the Depository container will happen that is necessary for the
     * Service Provider.
     *
     * @return void
     */
    public function register(): void;

    /**
     * Setter method to pass in the current Depository object for registering
     * items.
     *
     * @param Depository $depository    The Depository object for registering
     *                                  items.
     *
     * @return void
     */
    public function setDepository(Depository $depository): void;
}