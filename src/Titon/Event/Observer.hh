<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Event;

use \ReflectionFunction;
use \ReflectionMethod;

/**
 * The Observer class represents a single callback in the event stack.
 * It will be executed anytime an event is triggered that the observer is subscribed for.
 *
 * @package Titon\Event
 */
class Observer {

    /**
     * Is the callback asynchronous?
     *
     * @var bool
     */
    protected bool $async = false;

    /**
     * The callback to execute.
     *
     * @var \Titon\Event\ObserverCallback
     */
    protected ObserverCallback $callback;

    /**
     * Has the callback been executed.
     *
     * @var bool
     */
    protected bool $executed = false;

    /**
     * Should the callback be executed one time only?
     *
     * @var bool
     */
    protected bool $once;

    /**
     * The priority order for the observer.
     *
     * @var int
     */
    protected int $priority;

    /**
     * Setup the observer and auto-detect if the callback is async.
     *
     * @param \Titon\Event\ObserverCallback $callback
     * @param int $priority
     * @param bool $once
     */
    public function __construct(ObserverCallback $callback, int $priority, bool $once) {
        $this->callback = $callback;
        $this->priority = $priority;
        $this->once = $once;

        if (is_array($callback)) {
            $this->async = (new ReflectionMethod($callback[0], $callback[1]))->isAsync();
        } else {
            $this->async = (new ReflectionFunction($callback))->isAsync();
        }
    }

    /**
     * Asynchronously execute the callback and return the response.
     *
     * @param \Titon\Event\Event $event
     * @return Awaitable<mixed>
     */
    public async function asyncExecute(Event $event): Awaitable<mixed> {
        $this->executed = true;

        // UNSAFE
        // As the callback return type is not Awaitable<mixed> but simply mixed
        return await call_user_func($this->getCallback(), $event);
    }

    /**
     * Execute the callback and return the response.
     *
     * @param \Titon\Event\Event $event
     * @return mixed
     */
    public function execute(Event $event): mixed {
        $this->executed = true;

        return call_user_func($this->getCallback(), $event);
    }

    /**
     * Return the callback.
     *
     * @return \Titon\Event\ObserverCallback
     */
    public function getCallback(): ObserverCallback {
        return $this->callback;
    }

    /**
     * Return the class name, method, or function for the callback.
     *
     * @return string
     */
    public function getCaller(): string {
        $caller = '';
        $callback = $this->getCallback();

        is_callable($callback, true, $caller);

        if ($caller === 'Closure::__invoke') {
            $caller = '{closure}';
        }

        return $caller;
    }

    /**
     * Return the priority number.
     *
     * @return int
     */
    public function getPriority(): int {
        return $this->priority;
    }

    /**
     * Return true if the callback has executed.
     *
     * @return bool
     */
    public function hasExecuted(): bool {
        return $this->executed;
    }

    /**
     * Return true if the callback is asynchronous.
     *
     * @return bool
     */
    public function isAsync(): bool {
        return $this->async;
    }

    /**
     * Return true if the callback should execute once.
     *
     * @return bool
     */
    public function isOnce(): bool {
        return $this->once;
    }

}
