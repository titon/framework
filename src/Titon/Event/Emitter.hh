<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Event;

use Titon\Event\Exception\InvalidObserverException;
use \Closure;

type CallStack = shape('priority' => int, 'once' => bool, 'callback' => string, 'time' => int);
type CallStackList = Vector<CallStack>;
type EventList = Vector<Event>;
type Observer = shape('priority' => int, 'once' => bool, 'callback' => ObserverCallback);
type ObserverList = Vector<Observer>;
type ObserverContainer = Map<string, ObserverList>;
type ObserverCallback = (function(...): mixed);

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
     * @type \Titon\Event\ObserverContainer
     */
    protected ObserverContainer $_observers = Map {};

    /**
     * Notify all sorted observers by priority about an event.
     * A list of parameters can be defined that will be passed to each observer.
     *
     * @uses Titon\Event\Event
     *
     * @param string $event
     * @param array<mixed> $params
     * @return \Titon\Event\Event
     */
    public function emit(string $event, array<mixed> $params = []): Event {
        $object = new Event($event, $this->getCallStack($event));
        $turnOff = Vector {};

        // Add event as the 1st argument
        array_unshift($params, $object);

        // Loop through each observer
        foreach ($this->getSortedObservers($event) as $observer) {
            $response = call_user_func_array($observer['callback'], $params);

            // If the response is null/void (no return from callback) or true, don't do anything.
            // Else stop propagation and set the response state so that it may be used outside of the emitter.
            if ($response !== null && $response !== true) {
                $object->stop()->setState($response);
            }

            if ($observer['once']) {
                $turnOff[] = $observer['callback'];
            }

            if ($object->isStopped()) {
                break;
            }

            $object->next();
        }

        // We must do this as you can't remove keys while iterating
        foreach ($turnOff as $callback) {
            $this->off($event, $callback);
        }

        return $object;
    }

    /**
     * Emit multiple events at once by passing a list of event names, or event names separated by a space.
     * If a `*` is provided in the key, a wildcard match will occur.
     *
     * @param mixed $event
     * @param array<mixed> $params
     * @return \Titon\Event\EventList
     */
    public function emitMany(mixed $event, array<mixed> $params = []): EventList {
        $objects = Vector {};

        foreach ($this->_resolveEvents($event) as $event) {
            $objects[] = $this->emit($event, $params);
        }

        return $objects;
    }

    /**
     * Flush all observers or event specific observers.
     *
     * @param string $event
     * @return $this
     */
    public function flush(string $event = ''): this {
        if (!$event) {
            $this->_observers->clear();
        } else {
            $this->_observers->remove($event);
        }

        return $this;
    }

    /**
     * Return the call stack (order of priority) for an event.
     *
     * @param string $event
     * @return \Titon\Event\CallStackList
     */
    public function getCallStack(string $event): CallStackList {
        $stack = Vector {};

        if ($this->hasObservers($event)) {
            foreach ($this->getSortedObservers($event) as $observer) {
                $method = '{closure}';

                // Use `is_callable()` to fetch the callable name
                if (!$observer['callback'] instanceof Closure) {
                    is_callable($observer['callback'], true, $method);
                }

                $stack[] = shape(
                    'priority' => $observer['priority'],
                    'once' => $observer['once'],
                    'callback' => $method,
                    'time' => 0
                );
            }
        }

        return $stack;
    }

    /**
     * Return all the currently registered events keys.
     *
     * @return Vector<string>
     */
    public function getEventKeys(): Vector<string> {
        return $this->_observers->keys();
    }

    /**
     * Return all observers for an event.
     *
     * @param string $event
     * @return \Titon\Event\ObserverList
     */
    public function getObservers(string $event): ObserverList {
        if ($this->hasObservers($event)) {
            return $this->_observers[$event];
        }

        return Vector {};
    }

    /**
     * Return all observers for an event sorted by priority.
     *
     * @param string $event
     * @return \Titon\Event\ObserverList
     */
    public function getSortedObservers(string $event): ObserverList {
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
        return $this->_observers->contains($event);
    }

    /**
     * Register an event callback or listener to only trigger once and then remove itself from the observer list.
     *
     * @param string $event
     * @param mixed $callback
     * @param int $priority
     * @return $this
     */
    public function once(string $event, mixed $callback, int $priority = 0): this {
        return $this->on($event, $callback, $priority, true);
    }

    /**
     * Register an event callback or a listener.
     *
     * @param string $event
     * @param mixed $callback
     * @param int $priority
     * @param bool $once
     * @return $this
     * @throws \Titon\Event\Exception\InvalidObserverException
     */
    public function on(string $event, mixed $callback, int $priority = 0, bool $once = false): this {
        if ($callback instanceof Listener) {
            $this->registerListener($callback);

        } else if (is_callable($callback)) {
            invariant(is_callable($callback), 'Callback must be callable');

            // UNSAFE
            $this->register($event, $callback, $priority, $once);

        } else {
            throw new InvalidObserverException('Observer must be a callable or a Listener');
        }

        return $this;
    }

    /**
     * Remove an event callback or a listener.
     *
     * @param string $event
     * @param mixed $callback
     * @return $this
     * @throws \Titon\Event\Exception\InvalidObserverException
     */
    public function off(string $event, mixed $callback): this {
        if ($callback instanceof Listener) {
            $this->removeListener($callback);

        } else if (is_callable($callback)) {
            invariant(is_callable($callback), 'Callback must be callable');

            // UNSAFE
            $this->remove($event, $callback);

        } else {
            throw new InvalidObserverException('Observer must be a callable or a Listener');
        }

        return $this;
    }

    /**
     * Register a callback (observer) for an event.
     * A priority can be defined to change the order of execution.
     *
     * @param string $event
     * @param \Titon\Event\ObserverCallback $callback
     * @param int $priority
     * @param bool $once
     * @return $this
     */
    public function register(string $event, ObserverCallback $callback, int $priority = 0, bool $once = false): this {
        if (!$this->_observers->contains($event)) {
            $this->_observers[$event] = Vector {};
        }

        if (!$priority) {
            $priority = count($this->_observers[$event]) + self::DEFAULT_PRIORITY;
        }

        $this->_observers[$event][] = shape(
            'callback' => $callback,
            'priority' => $priority,
            'once' => $once
        );

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
                // UNSAFE
                $this->register($event, inst_meth($listener, (string) $opt['method']), (int) $opt['priority'], (bool) $opt['once']);
            }
        }

        return $this;
    }

    /**
     * Remove a callback from an event.
     *
     * @param string $event
     * @param \Titon\Event\ObserverCallback $callback
     * @return $this
     */
    public function remove(string $event, ObserverCallback $callback): this {
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
                // UNSAFE
                $this->remove($event, inst_meth($listener, (string) $opt['method']));
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
        if (!$options instanceof Vector) {
            $options = new Vector([$options]);
        }

        invariant($options instanceof Vector, 'Options must be a vector');

        $parsed = Vector {};

        foreach ($options as $option) {
            $settings = Map {
                'method' => '',
                'priority' => 0,
                'once' => false
            };

            if (is_string($option)) {
                $settings['method'] = $option;

            } else if ($option instanceof Map) {
                $settings->setAll($option);
            }

            $parsed[] = $settings;
        }

        return $parsed;
    }

    /**
     * Resolve an event key into multiple events by checking for space delimiters and wildcard matches.
     *
     * @param string|array $events
     * @return Vector<string>
     */
    protected function _resolveEvents(mixed $events): Vector<string> {
        $found = Vector {};

        if (!$events instanceof Traversable) {
            $events = explode(' ', (string) $events);
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