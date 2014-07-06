<?hh //strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Event;

use Titon\Utility\Hash;

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
     * @type array
     */
    protected $_data = [];

    /**
     * The event key.
     *
     * @type string
     */
    protected $_key;

    /**
     * The current index in the call stack.
     *
     * @type int
     */
    protected $_index = 0;

    /**
     * Has the event stopped? This will cancel upcoming listeners.
     *
     * @type bool
     */
    protected $_stopped = false;

    /**
     * The call stack in order of priority.
     *
     * @type array
     */
    protected $_stack = [];

    /**
     * The last state before the object was stopped.
     * This value is automatically set by the emitter using the callback response.
     *
     * @type mixed
     */
    protected $_state = true;

    /**
     * The timestamp of when the event started.
     *
     * @type int
     */
    protected $_time;

    /**
     * Initialize the event and pass information from the Emitter.
     *
     * @param string $key
     * @param array $stack
     */
    public function __construct($key, array $stack = []) {
        $this->_key = $key;
        $this->_stack = $stack;
        $this->_time = time();
    }

    /**
     * Return the call stack in order of priority.
     *
     * @return array
     */
    public function getCallStack() {
        return $this->_stack;
    }

    /**
     * Return the data defined by key, or return all data.
     *
     * @param string $key
     * @return mixed
     */
    public function getData($key = null) {
        return Hash::get($this->_data, $key);
    }

    /**
     * Return the current index in the call stack.
     *
     * @return int
     */
    public function getIndex() {
        return $this->_index;
    }

    /**
     * Return the event key.
     *
     * @return string
     */
    public function getKey() {
        return $this->_key;
    }

    /**
     * Return the response state.
     *
     * @return mixed
     */
    public function getState() {
        return $this->_state;
    }

    /**
     * Return the event timestamp.
     *
     * @return int
     */
    public function getTime() {
        return $this->_time;
    }

    /**
     * Check if the event has stopped.
     *
     * @return bool
     */
    public function isStopped() {
        return $this->_stopped;
    }

    /**
     * Jump to the next call stack.
     * If next reaches the end, stop the event.
     *
     * @return $this
     */
    public function next() {
        $index = $this->_index;
        $nextIndex = $index + 1;

        if (isset($this->_stack[$nextIndex])) {
            $this->_stack[$index]['time'] = time();
            $this->_index = $nextIndex;

        } else {
            $this->stop();
        }

        return $this;
    }

    /**
     * Set data to pass to the next observer.
     *
     * @param string $key
     * @param mixed $value
     * @return $this
     */
    public function setData($key, $value) {
        $this->_data = Hash::set($this->_data, $key, $value);

        return $this;
    }

    /**
     * Set the response state.
     *
     * @param mixed $state
     * @return $this
     */
    public function setState($state) {
        $this->_state = $state;

        return $this;
    }

    /**
     * Stop the event.
     *
     * @return $this
     */
    public function stop() {
        $this->_stopped = true;

        return $this;
    }

}