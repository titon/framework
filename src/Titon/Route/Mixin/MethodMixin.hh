<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Route\Mixin;

type MethodList = Vector<string>;

/**
 * Provides functionality for HTTP methods.
 *
 * @package Titon\Route\Mixin
 */
trait MethodMixin {

    /**
     * The types of acceptable HTTP methods (defaults to all).
     *
     * @var \Titon\Route\Mixin\MethodList
     */
    protected MethodList $_methods = Vector {};

    /**
     * Add an HTTP method to match against.
     *
     * @param string $method
     * @return $this
     */
    public function addMethod(string $method): this {
        if (!in_array($method, $this->_methods)) {
            $this->_methods[] = strtolower($method);
        }

        return $this;
    }

    /**
     * Add multiple HTTP methods to match against.
     *
     * @param \Titon\Route\Mixin\MethodList $methods
     * @return $this
     */
    public function addMethods(MethodList $methods): this {
        foreach ($methods as $method) {
            $this->addMethod($method);
        }

        return $this;
    }

    /**
     * Return the HTTP method.
     *
     * @return \Titon\Route\MethodList
     */
    public function getMethods(): MethodList {
        return $this->_methods;
    }

    /**
     * Set the list of HTTP methods to match against.
     *
     * @param \Titon\Route\Mixin\MethodList $methods
     * @return $this
     */
    public function setMethods(MethodList $methods): this {
        $this->_methods = $methods->map(fun('strtolower'));

        return $this;
    }

}