<?hh //strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Event;

use Titon\Event\Exception\InvalidCallbackException;
use \Closure;

/**
 * The Emitter manages the registering and removing of callbacks (observers).
 * A dispatched (or emitted) event will cycle through and trigger all callbacks.
 *
 * @package Titon\Event
 */
class Emitter {

    const DEFAULT_PRIORITY = 100;

    /**
     * Registered observers per event.
     *
     * @type array
     */
    protected $_observers = [];

    /**
     * Notify all observers in priority order about the event.
     * Pass an event object to each callback that can be stopped at any time.
     *
     * Multiple events can be emitted by separating event names with a space or providing a * wildcard.
     *
     * @uses Titon\Event\Event
     *
     * @param string|array $event
     * @param array $params
     * @return \Titon\Event\Event|\Titon\Event\Event[]
     */
    public function emit($event, array $params = []) {
        $events = $this->_resolveEvents($event);
        $objects = [];

        foreach ($events as $event) {
            $object = new Event($event, $this->getCallStack($event));

            array_unshift($params, $object);

            foreach ($this->getSortedObservers($event) as $observer) {
                $response = call_user_func_array($observer['callback'], $params);

                // If the response is null/void (no return from callback) or true, don't do anything.
                // Else stop propagation and set the response state so that it may be used outside of the emitter.
                if ($response !== null && $response !== true) {
                    $object->stop()->setState($response);
                }

                if ($object->isStopped()) {
                    break;

                } else if ($observer['once']) {
                    $this->off($event, $observer['callback']);
                }

                $object->next();
            }

            $objects[] = $object;
        }

        if (count($events) === 1 && isset($objects[0])) {
            return $objects[0];
        }

        return $objects;
    }

    /**
     * Flush all observers or event specific observers.
     *
     * @param string $event
     * @return $this
     */
    public function flush($event = null) {
        if (!$event) {
            $this->_observers = [];
        } else {
            unset($this->_observers[$event]);
        }

        return $this;
    }

    /**
     * Return the call stack (order of priority) for an event.
     *
     * @param string $event
     * @return array
     */
    public function getCallStack($event) {
        $stack = [];

        if ($this->hasObservers($event)) {
            foreach ($this->getSortedObservers($event) as $observer) {
                if ($observer['callback'] instanceof Closure) {
                    $method = '{closure}';
                } else {
                    is_callable($observer['callback'], true, $method); // Used to fetch the callable name
                }

                $stack[] = ['callback' => $method] + $observer;
            }
        }

        return $stack;
    }

    /**
     * Return all the currently registered events keys.
     *
     * @return string[]
     */
    public function getEvents() {
        return array_keys($this->_observers);
    }

    /**
     * Return all observers for an event.
     *
     * @param string $event
     * @return array
     */
    public function getObservers($event) {
        if ($this->hasObservers($event)) {
            return $this->_observers[$event];
        }

        return [];
    }

    /**
     * Return all observers for an event sorted by priority.
     *
     * @param string $event
     * @return array
     */
    public function getSortedObservers($event) {
        if ($obs = $this->getObservers($event)) {
            usort($obs, function($a, $b) {
                if ($a['priority'] == $b['priority']) {
                    return 0;
                }

                return ($a['priority'] < $b['priority']) ? -1 : 1;
            });

            return $obs;
        }

        return [];
    }

    /**
     * Return true if the event has observers.
     *
     * @param string $event
     * @return bool
     */
    public function hasObservers($event) {
        return isset($this->_observers[$event]);
    }

    /**
     * Register an event callback or listener to only trigger once and then remove itself from the observer list.
     *
     * @param string $event
     * @param callable $callback
     * @param mixed $options
     * @return $this
     */
    public function once($event, $callback, $options = null) {
        if (!is_array($options)) {
            $options = ['priority' => $options];
        }

        return $this->on($event, $callback, ['once' => true] + $options);
    }

    /**
     * Register an event callback or a listener.
     *
     * @param string $event
     * @param callable $callback
     * @param mixed $options
     * @return $this
     */
    public function on($event, $callback, $options = null) {
        if ($callback instanceof Listener) {
            $this->registerListener($callback);
        } else {
            $this->register($event, $callback, $options);
        }

        return $this;
    }

    /**
     * Remove an event callback or a listener.
     *
     * @param string $event
     * @param callable $callback
     * @return $this
     */
    public function off($event, $callback) {
        if ($callback instanceof Listener) {
            $this->removeListener($callback);
        } else {
            $this->remove($event, $callback);
        }

        return $this;
    }

    /**
     * Register a callback (observer) for an event.
     * A priority can be defined to change the order of execution.
     *
     * @param string $event
     * @param callable $callback
     * @param mixed $options
     * @return $this
     * @throws \Titon\Event\Exception\InvalidCallbackException
     */
    public function register($event, $callback, $options = null) {
        if (!is_callable($callback, false)) {
            throw new InvalidCallbackException(sprintf('Invalid observer for %s event, must be callable', $event));
        }

        if (empty($this->_observers[$event])) {
            $this->_observers[$event] = [];
        }

        if (!is_array($options)) {
            $options = ['priority' => $options];
        }

        $options = $options + [
            'priority' => null,
            'once' => false
        ];

        if (!$options['priority']) {
            $options['priority'] = count($this->_observers[$event]) + self::DEFAULT_PRIORITY;
        }

        unset($options['method']);

        $this->_observers[$event][] = ['callback' => $callback] + $options;

        return $this;
    }

    /**
     * Register multiple events that are provided from a listener object.
     *
     * @param \Titon\Event\Listener $listener
     * @return $this
     */
    public function registerListener(Listener $listener) {
        foreach ($listener->registerEvents() as $event => $options) {
            foreach ($this->_parseOptions($options) as $opt) {
                $this->register($event, [$listener, $opt['method']], $opt);
            }
        }

        return $this;
    }

    /**
     * Remove a callback from an event.
     *
     * @param string $event
     * @param callable $callback
     * @return $this
     */
    public function remove($event, $callback) {
        foreach ($this->getObservers($event) as $i => $observer) {
            if ($observer['callback'] === $callback) {
                unset($this->_observers[$event][$i]);
            }
        }

        $this->_observers[$event] = array_values($this->_observers[$event]);

        return $this;
    }

    /**
     * Remove multiple callbacks from an event from a listener object.
     *
     * @param \Titon\Event\Listener $listener
     * @return $this
     */
    public function removeListener(Listener $listener) {
        foreach ($listener->registerEvents() as $event => $options) {
            foreach ($this->_parseOptions($options) as $opt) {
                $this->remove($event, [$listener, $opt['method']]);
            }
        }

        return $this;
    }

    /**
     * Parse the options from a listener into an indexed array of object method callbacks.
     *
     * @param array|string $options
     * @return array
     */
    protected function _parseOptions($options) {
        if (!is_array($options) || !isset($options[0])) {
            $options = [$options];
        }

        foreach ($options as $index => $opt) {
            if (!is_array($opt)) {
                $opt = ['method' => $opt];
            }

            $options[$index] = $opt + [
                'method' => '',
                'priority' => null,
                'once' => false
            ];
        }

        return $options;
    }

    /**
     * Resolve an event key into multiple events by checking for space delimiters and wildcard matches.
     *
     * @param string|array $events
     * @return array
     */
    protected function _resolveEvents($events) {
        $found = [];

        if (is_string($events)) {
            $events = explode(' ', $events);
        }

        foreach ($events as $event) {
            if (strpos($event, '*') !== false) {
                $pattern = '/^' . str_replace('*', '([-\w]+)', $event) . '$/i';

                foreach ($this->_observers as $eventKey => $observers) {
                    if (preg_match($pattern, $eventKey)) {
                        $found[] = $eventKey;
                    }
                }
            } else {
                $found[] = $event;
            }
        }

        return $found;
    }

}