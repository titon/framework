<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Event;

use \Closure;
use \ReflectionFunction;
use \ReflectionMethod;

/**
 * The Observer class represents a single callback in the event stack.
 * It will be executed anytime an event is triggered that the observer is registered for.
 *
 * @package Titon\Event
 */
class Observer {

    /**
     * Is the callback asynchronous?
     *
     * @type bool
     */
    protected bool $_async = false;

    /**
     * The callback to execute.
     *
     * @type \Titon\Event\ObserverCallback
     */
    protected ObserverCallback $_callback;

    /**
     * Has the callback been executed.
     *
     * @type bool
     */
    protected bool $_executed = false;

    /**
     * Should the callback be executed one time only?
     *
     * @type bool
     */
    protected bool $_once;

    /**
     * The priority order for the observer.
     *
     * @type int
     */
    protected int $_priority;

    /**
     * Setup the observer and auto-detect if the callback is async.
     *
     * @param \Titon\Event\ObserverCallback $callback
     * @param int $priority
     * @param bool $once
     */
    public function __construct(ObserverCallback $callback, int $priority, bool $once) {
        $this->_callback = $callback;
        $this->_priority = $priority;
        $this->_once = $once;

        if (is_array($callback)) {
            $this->_async = (new ReflectionMethod($callback[0], $callback[1]))->isAsync();
        } else {
            $this->_async = (new ReflectionFunction($callback))->isAsync();
        }
    }

    /**
     * Asynchronously execute the callback and return the response.
     *
     * @param array<mixed> $params
     * @return Awaitable<mixed>
     */
    async public function asyncExecute(array<mixed> $params): Awaitable<mixed> {
        return $this->execute($params);
    }

    /**
     * Execute the callback and return the response.
     *
     * @param array<mixed> $params
     * @return mixed
     */
    public function execute(array<mixed> $params): mixed {
        $this->_executed = true;

        return call_user_func_array($this->getCallback(), $params);
    }

    /**
     * Return the callback.
     *
     * @return \Titon\Event\ObserverCallback
     */
    public function getCallback(): ObserverCallback {
        return $this->_callback;
    }

    /**
     * Return the class name, method, or function for the callback.
     *
     * @return string
     */
    public function getCaller(): string {
        $caller = '{closure}';
        $callback = $this->getCallback();

        // Use `is_callable()` to fetch the callable name
        if (!$callback instanceof Closure) {
            is_callable($callback, true, $caller);
        }

        return $caller;
    }

    /**
     * Return the priority number.
     *
     * @return int
     */
    public function getPriority(): int {
        return $this->_priority;
    }

    /**
     * Return true if the callback has executed.
     *
     * @return bool
     */
    public function hasExecuted(): bool {
        return $this->_executed;
    }

    /**
     * Return true if the callback is asynchronous.
     *
     * @return bool
     */
    public function isAsync(): bool {
        return $this->_async;
    }

    /**
     * Return true if the callback should execute once.
     *
     * @return bool
     */
    public function isOnce(): bool {
        return $this->_once;
    }

}