<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Route;

use Titon\Route\Exception\NoMatchException;
use \ReflectionFunction;

type RouteCallback = (function(...): mixed);

/**
 * The CallbackRoute works in a similar fashion to the default Route with the only difference being
 * that a callback (Closure, etc) is used for dispatching instead of an action.
 *
 * @package Titon\Route
 */
class CallbackRoute extends Route {

    /**
     * The callback to execute during dispatch.
     *
     * @var \Titon\Route\RouteCallback
     */
    protected RouteCallback $_callback;

    /**
     * Store the tokenized URL to match and the callback to dispatch to.
     *
     * @param string $path
     * @param \Titon\Route\RouteCallback $callback
     */
    public function __construct(string $path, RouteCallback $callback) {
        $this->_callback = $callback;

        parent::__construct($path, 'CallbackRoute@noop');
    }

    /**
     * {@inheritdoc}
     */
    public function dispatch(): mixed {
        if (!$this->isMatched()) {
            throw new NoMatchException('Route cannot be dispatched unless it has been matched');
        }

        $callback = new ReflectionFunction($this->getCallback());

        return $callback->invokeArgs($this->_getArguments($callback)->toArray());
    }

    /**
     * Return the callback function.
     *
     * @return \Titon\Route\RouteCallback
     */
    public function getCallback(): RouteCallback {
        return $this->_callback;
    }

    /**
     * Set the callback function.
     *
     * @param \Titon\Route\RouteCallback $callback
     * @return $this
     */
    public function setCallback(RouteCallback $callback): this {
        $this->_callback = $callback;

        return $this;
    }

}