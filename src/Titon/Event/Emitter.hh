<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Event;

type CallStackList = Vector<string>;
type EventMap = Map<string, Event>;
type ObserverList = Vector<Observer>;
type ObserverContainer = Map<string, ObserverList>;
type ObserverCallback = (function(...): mixed);

/**
 * The Emitter manages the subscribing and removing of observers (and listeners).
 * An emitted event will cycle through and trigger all observers.
 *
 * @package Titon\Event
 */
class Emitter {

    const int AUTO_PRIORITY = 0;
    const int DEFAULT_PRIORITY = 100;

    /**
     * Registered observers per event.
     *
     * @var \Titon\Event\ObserverContainer
     */
    protected ObserverContainer $_observers = Map {};

    /**
     * Notify all synchronous and asynchronous observers, sorted by priority, about an event.
     * A list of parameters can be defined that will be passed to each observer.
     *
     * @param \Titon\Event\Event $event
     * @param \Titon\Event\ParamList $params
     * @return \Titon\Event\Event
     */
    public function dispatch(Event $event, ParamList $params): Event {
        $key = $event->getKey();

        // Set call stack
        $event->setCallStack($this->getCallStack($key));

        // Add event as the 1st param
        array_unshift($params, $event);

        // Group the observers
        $observers = $this->getSortedObservers($key);
        $syncObservers = Vector {};
        $asyncObservers = Vector {};

        foreach ($observers as $observer) {
            if ($observer->isAsync()) {
                $asyncObservers[] = $observer;
            } else {
                $syncObservers[] = $observer;
            }
        }

        // Notify observers
        $this->_notifyObservers($syncObservers, $event, $params);
        $this->_notifyObserversAsync($asyncObservers, $event, $params)->getWaitHandle()->join();

        return $event;
    }

    /**
     * Emit a single event defined by an event name. Can pass an optional list of parameters.
     *
     * @uses Titon\Event\Event
     *
     * @param string $event
     * @param \Titon\Event\ParamList $params
     * @return \Titon\Event\Event
     */
    public function emit(string $event, ParamList $params): Event {
        return $this->dispatch(new Event($event), $params);
    }

    /**
     * Emit multiple events at once by passing a list of event names, or event names separated by a space.
     * If a `*` is provided in the key, a wildcard match will occur.
     *
     * @param mixed $event
     * @param \Titon\Event\ParamList $params
     * @return \Titon\Event\EventMap
     */
    public function emitMany(mixed $event, ParamList $params): EventMap {
        $objects = Map {};

        foreach ($this->_resolveEventKeys($event) as $event) {
            $objects[$event] = $this->emit($event, $params);
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

        foreach ($this->getSortedObservers($event) as $observer) {
            $stack[] = $observer->getCaller();
        }

        return $stack;
    }

    /**
     * Return all the currently subscribed events keys.
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
        $observers = $this->getObservers($event);

        if ($observers) {
            usort($observers, (Observer $a, Observer $b) ==> {
                if ($a->getPriority() == $b->getPriority()) {
                    return 0;
                }

                return ($a->getPriority() < $b->getPriority()) ? -1 : 1;
            });
        }

        return $observers;
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
     * Register a callback (observer) for an event.
     * A priority can be defined to change the order of execution.
     *
     * @param string $event
     * @param \Titon\Event\ObserverCallback $callback
     * @param int $priority
     * @param bool $once
     * @return $this
     */
    public function subscribe(string $event, ObserverCallback $callback, int $priority = self::AUTO_PRIORITY, bool $once = false): this {
        if (!$this->hasObservers($event)) {
            $this->_observers[$event] = Vector {};
        }

        if ($priority === self::AUTO_PRIORITY) {
            $priority = count($this->_observers[$event]) + self::DEFAULT_PRIORITY;
        }

        $this->_observers[$event][] = new Observer($callback, $priority, $once);

        return $this;
    }

    /**
     * Register multiple events that are provided from a listener object.
     *
     * @param \Titon\Event\Listener $listener
     * @return $this
     */
    public function subscribeListener(Listener $listener): this {
        foreach ($listener->subscribeEvents() as $event => $options) {
            foreach ($this->_parseOptions($options) as $opt) {
                // UNSAFE
                // Since inst_meth() requires literal strings and we are passing variables
                $this->subscribe($event, inst_meth($listener, $opt['method']), $opt['priority'], $opt['once']);
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
            if ($observer->getCallback() === $callback) {
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
        foreach ($listener->subscribeEvents() as $event => $options) {
            foreach ($this->_parseOptions($options) as $opt) {
                // UNSAFE
                // Since inst_meth() requires literal strings and we are passing variables
                $this->remove($event, inst_meth($listener, $opt['method']));
            }
        }

        return $this;
    }

    /**
     * Notify the observer by executing the callback with the defined params.
     * Can optionally stop the event and set a state based on the callbacks response.
     *
     * @param \Titon\Event\Observer $observer
     * @param \Titon\Event\Event $event
     * @param \Titon\Event\ParamList $params
     * @return bool
     */
    protected function _executeObserver(Observer $observer, Event $event, ParamList $params): bool {
        if ($event->isStopped()) {
            return false;

        } else if ($observer->isOnce() && $observer->hasExecuted()) {
            return true;
        }

        return $this->_handleExecution($event, $observer->execute($params));
    }

    /**
     * Asynchronously notify the observer.
     *
     * @param \Titon\Event\Observer $observer
     * @param \Titon\Event\Event $event
     * @param \Titon\Event\ParamList $params
     * @return bool
     */
    protected async function _executeObserverAsync(Observer $observer, Event $event, ParamList $params): Awaitable<bool> {
        if ($event->isStopped()) {
            return false;

        } else if ($observer->isOnce() && $observer->hasExecuted()) {
            return true;
        }

        $response = await $observer->asyncExecute($params);

        return $this->_handleExecution($event, $response);
    }

    /**
     * Handle the response of an executed observer callback.
     * If the response is null, void (no return from callback), or true, don't do anything.
     * Else stop propagation and set the response state so that it may be used outside of the emitter.
     *
     * @param \Titon\Event\Event $event
     * @param mixed $response
     * @return bool
     */
    protected function _handleExecution(Event $event, mixed $response): bool {
        if ($response !== null && $response !== true) {
            $event->stop()->setState($response);
        } else {
            $event->next();
        }

        return true;
    }

    /**
     * Loop over a list of observers and notify them for the defined event, with the defined params.
     *
     * @param \Titon\Event\ObserverList $observers
     * @param \Titon\Event\Event $event
     * @param \Titon\Event\ParamList $params
     * @return bool
     */
    protected function _notifyObservers(ObserverList $observers, Event $event, ParamList $params): bool {
        foreach ($observers as $observer) {
            $this->_executeObserver($observer, $event, $params);

            if ($event->isStopped()) {
                return false;
            }
        }

        return true;
    }

    /**
     * Loop over a list of async observers and execute them in parallel using an await handler.
     *
     * @param \Titon\Event\ObserverList $observers
     * @param \Titon\Event\Event $event
     * @param \Titon\Event\ParamList $params
     * @return Awaitable<mixed>
     */
    protected async function _notifyObserversAsync(ObserverList $observers, Event $event, ParamList $params): Awaitable<mixed> {
        if ($event->isStopped()) {
            return false; // Exit early if non-async stopped propagation
        }

        $handles = Vector {};

        foreach ($observers as $observer) {
            $handles[] = $this->_executeObserverAsync($observer, $event, $params)->getWaitHandle();
        }

        await GenVectorWaitHandle::create($handles);

        return true;
    }

    /**
     * Parse the options from a listener into an indexed array of object method callbacks.
     *
     * @param array|string $options
     * @return Vector<ListenerOption>
     */
    protected function _parseOptions(mixed $options): Vector<ListenerOption> {
        if (!$options instanceof Vector) {
            $options = new Vector([$options]);
        }

        invariant($options instanceof Vector, 'Event options must be a vector');

        $parsed = Vector {};

        foreach ($options as $option) {
            $settings = shape(
                'method' => '',
                'priority' => self::AUTO_PRIORITY,
                'once' => false
            );

            if (is_string($option)) {
                $settings['method'] = $option;

            } else if ($option instanceof Map) {
                $settings = array_merge($settings, $option->toArray());
            }

            $parsed[] = $settings;
        }

        return $parsed;
    }

    /**
     * Resolve an event key into multiple events by checking for space delimiters and wildcard matches.
     *
     * @param mixed $events
     * @return Vector<string>
     */
    protected function _resolveEventKeys(mixed $events): Vector<string> {
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
