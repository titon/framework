<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Route;

use Titon\Route\Exception\NoMatchException;
use \ReflectionFunction;
use \Closure;

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
     * @var \Closure
     */
    protected Closure $callback;

    /**
     * Store the tokenized URL to match and the callback to dispatch to.
     *
     * @param string $path
     * @param \Closure $callback
     */
    public function __construct(string $path, Closure $callback) {
        $this->callback = $callback;

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

        return $callback->invokeArgs($this->getArguments($callback));
    }

    /**
     * Return the callback function.
     *
     * @return \Closure
     */
    public function getCallback(): Closure {
        return $this->callback;
    }

    /**
     * Set the callback function.
     *
     * @param \Closure $callback
     * @return $this
     */
    public function setCallback(Closure $callback): this {
        $this->callback = $callback;

        return $this;
    }

}
