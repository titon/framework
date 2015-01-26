<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Event;

use Titon\Common\DataMap;
use Titon\Utility\Col;

/**
 * An object representing the current event being dispatched.
 * The event can be stopped at any time during the cycle.
 *
 * @package Titon\Event
 */
class Event {

    /**
     * Data to persist between observers.
     *
     * @var \Titon\Common\DataMap
     */
    protected DataMap $_data = Map {};

    /**
     * The event key.
     *
     * @var string
     */
    protected string $_key;

    /**
     * The current count of how many observers have been notified.
     *
     * @var int
     */
    protected int $_index = 0;

    /**
     * Has the event stopped? This will cancel upcoming observers.
     *
     * @var bool
     */
    protected bool $_stopped = false;

    /**
     * The call stack in order of priority.
     *
     * @var \Titon\Event\CallStackList
     */
    protected CallStackList $_stack = Vector {};

    /**
     * The last state before the object was stopped.
     * This value is automatically set by the emitter using the callback response.
     *
     * @var mixed
     */
    protected mixed $_state = true;

    /**
     * The timestamp of when the event started.
     *
     * @var int
     */
    protected int $_time;

    /**
     * Initialize the event.
     *
     * @param string $key
     */
    public function __construct(string $key) {
        $this->_key = $key;
        $this->_time = time();
    }

    /**
     * Return the call stack in order of priority.
     *
     * @return \Titon\Event\CallStackList
     */
    public function getCallStack(): CallStackList {
        return $this->_stack;
    }

    /**
     * Return the data defined by key, or return all data.
     *
     * @param string $key
     * @return mixed
     */
    public function getData(string $key = ''): mixed {
        return Col::get($this->_data, $key);
    }

    /**
     * Return the current notify count.
     *
     * @return int
     */
    public function getIndex(): int {
        return $this->_index;
    }

    /**
     * Return the event key.
     *
     * @return string
     */
    public function getKey(): string {
        return $this->_key;
    }

    /**
     * Return the response state.
     *
     * @return mixed
     */
    public function getState(): mixed {
        return $this->_state;
    }

    /**
     * Return the event timestamp.
     *
     * @return int
     */
    public function getTime(): int {
        return $this->_time;
    }

    /**
     * Check if the event has stopped.
     *
     * @return bool
     */
    public function isStopped(): bool {
        return $this->_stopped;
    }

    /**
     * Increase the notify counter if the event has not stopped.
     *
     * @return $this
     */
    public function next(): this {
        if (!$this->isStopped()) {
            $this->_index++;
        }

        return $this;
    }

    /**
     * Set the call stack for the current event.
     *
     * @param \Titon\Event\CallStackList $stack
     * @return $this
     */
    public function setCallStack(CallStackList $stack): this {
        $this->_stack = $stack;

        return $this;
    }

    /**
     * Set data to pass to the next observer.
     *
     * @param string $key
     * @param mixed $value
     * @return $this
     */
    public function setData(string $key, mixed $value): this {
        Col::set($this->_data, $key, $value);

        return $this;
    }

    /**
     * Set the response state.
     *
     * @param mixed $state
     * @return $this
     */
    public function setState(mixed $state): this {
        $this->_state = $state;

        return $this;
    }

    /**
     * Stop the event.
     *
     * @return $this
     */
    public function stop(): this {
        $this->_stopped = true;

        return $this;
    }

}
