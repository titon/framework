<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Event;

use Titon\Utility\Traverse;
use \Closure;

/**
 * The Emitter manages the registering and removing of callbacks (observers).
 * A dispatched (or emitted) event will cycle through and trigger all callbacks.
 *
 * @package Titon\Event
 */
class Emitter {

    const int DEFAULT_PRIORITY = 100;

    /**
     * Registered observers per event.
     *
     * @type Map<string, Vector<Map<string, mixed>>>
     */
    protected Map<string, Vector<Map<string, mixed>>> $_observers = Map {};

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
    public function emit(mixed $event, array $params = []): mixed {
        $events = $this->_resolveEvents($event);
        $objects = Vector {};

        foreach ($events as $event) {
            $object = new Event($event, $this->getCallStack($event));
            $turnOff = Vector {};

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
                    $turnOff[] = $observer['callback'];
                }

                $object->next();
            }

            $objects[] = $object;

            // We must do this as you can't remove keys while iterating
            foreach ($turnOff as $callback) {
                $this->off($event, $callback);
            }
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
    public function flush(?string $event = null): this {
        if (!$event) {
            $this->_observers->clear();
        } else {
            unset($this->_observers[$event]);
        }

        return $this;
    }

    /**
     * Return the call stack (order of priority) for an event.
     *
     * @param string $event
     * @return Vector<Map<string, mixed>>
     */
    public function getCallStack(string $event): Vector<Map<string, mixed>> {
        $stack = Vector {};

        if ($this->hasObservers($event)) {
            foreach ($this->getSortedObservers($event) as $observer) {
                if ($observer['callback'] instanceof Closure) {
                    $method = '{closure}';
                } else {
                    is_callable($observer['callback'], true, $method); // Used to fetch the callable name
                }

                // Clone the map so we don't update the reference
                $options = $observer->toMap();
                $options['callback'] = $method;

                $stack[] = $options;
            }
        }

        return $stack;
    }

    /**
     * Return all the currently registered events keys.
     *
     * @return Vector<string>
     */
    public function getEvents(): Vector<string> {
        return $this->_observers->keys();
    }

    /**
     * Return all observers for an event.
     *
     * @param string $event
     * @return Vector<Map<string, mixed>>
     */
    public function getObservers(string $event): Vector<Map<string, mixed>> {
        if ($this->hasObservers($event)) {
            return $this->_observers[$event];
        }

        return Vector {};
    }

    /**
     * Return all observers for an event sorted by priority.
     *
     * @param string $event
     * @return Vector<Map<string, mixed>>
     */
    public function getSortedObservers(string $event): Vector<Map<string, mixed>> {
        if ($obs = $this->getObservers($event)) {
            usort($obs, function($a, $b) {
                if ($a['priority'] == $b['priority']) {
                    return 0;
                }

                return ($a['priority'] < $b['priority']) ? -1 : 1;
            });

            return $obs;
        }

        return Vector {};
    }

    /**
     * Return true if the event has observers.
     *
     * @param string $event
     * @return bool
     */
    public function hasObservers(string $event): bool {
        return isset($this->_observers[$event]);
    }

    /**
     * Register an event callback or listener to only trigger once and then remove itself from the observer list.
     *
     * @param string $event
     * @param mixed $callback
     * @param Map<string, mixed> $options
     * @return $this
     */
    public function once(string $event, mixed $callback, Map<string, mixed> $options = Map {}): this {
        $options['once'] = true;

        return $this->on($event, $callback, $options);
    }

    /**
     * Register an event callback or a listener.
     *
     * @param string $event
     * @param mixed $callback
     * @param Map<string, mixed> $options
     * @return $this
     */
    public function on(?string $event, mixed $callback, Map<string, mixed> $options = Map {}): this {
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
     * @param mixed $callback
     * @return $this
     */
    public function off(?string $event, mixed $callback): this {
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
     * @param Map<string, mixed> $options
     * @return $this
     * @throws \Titon\Event\Exception\InvalidCallbackException
     */
    public function register(string $event, callable $callback, Map<string, mixed> $options = Map {}): this {
        if (empty($this->_observers[$event])) {
            $this->_observers[$event] = Vector {};
        }

        $options = Traverse::merge(Map {
            'priority' => null,
            'once' => false
        }, $options);

        if (!$options['priority']) {
            $options['priority'] = count($this->_observers[$event]) + self::DEFAULT_PRIORITY;
        }

        $options['callback'] = $callback;

        unset($options['method']);

        $this->_observers[$event][] = $options;

        return $this;
    }

    /**
     * Register multiple events that are provided from a listener object.
     *
     * @param \Titon\Event\Listener $listener
     * @return $this
     */
    public function registerListener(Listener $listener): this {
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
    public function remove(string $event, callable $callback): this {
        $indices = Vector {};

        foreach ($this->getObservers($event) as $i => $observer) {
            if ($observer['callback'] === $callback) {
                $indices[] = $i;
            }
        }

        // We must do this as you can't remove keys while iterating
        foreach ($indices as $i) {
            $this->_observers[$event]->removeKey($i);
        }

        return $this;
    }

    /**
     * Remove multiple callbacks from an event from a listener object.
     *
     * @param \Titon\Event\Listener $listener
     * @return $this
     */
    public function removeListener(Listener $listener): this {
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
     * @return Vector<Map<string, mixed>>
     */
    protected function _parseOptions(mixed $options): Vector<Map<string, mixed>> {
        if (is_array($options) || $options instanceof Vector) { // Do not use is_traversable()
            $options = new Vector($options);
        } else {
            $options = new Vector([$options]);
        }

        foreach ($options as $index => $option) {
            $settings = Map {
                'method' => '',
                'priority' => null,
                'once' => false
            };

            if (is_string($option)) {
                $settings->set('method', $option);
            } else {
                $settings->setAll($option);
            }

            $options[$index] = $settings;
        }

        return $options;
    }

    /**
     * Resolve an event key into multiple events by checking for space delimiters and wildcard matches.
     *
     * @param string|array $events
     * @return Vector<string>
     */
    protected function _resolveEvents(mixed $events): Vector<string> {
        $found = Vector {};

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