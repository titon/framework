<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Event;

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
    protected ObserverContainer $observers = Map {};

    /**
     * Notify all synchronous and asynchronous observers, sorted by priority, about an event.
     *
     * @param \Titon\Event\Event $event
     * @return \Titon\Event\Event
     */
    public function emit(Event $event): Event {
        $key = $event->getKey();

        // Set call stack
        $event->setCallStack($this->getCallStack($key));

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
        $this->notifyObservers($syncObservers, $event);
        $this->notifyObserversAsync($asyncObservers, $event)->getWaitHandle()->join();

        return $event;
    }

    /**
     * Emit multiple events at once by passing a list of event objects.
     *
     * @param \Titon\Event\EventList $events
     * @return \Titon\Event\EventMap
     */
    public function emitMany(EventList $events): EventMap {
        $objects = Map {};

        foreach ($events as $event) {
            $objects[$event->getKey()] = $this->emit($event);
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
            $this->observers->clear();
        } else {
            $this->observers->remove($event);
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
        return $this->observers->keys();
    }

    /**
     * Return all observers for an event.
     *
     * @param string $event
     * @return \Titon\Event\ObserverList
     */
    public function getObservers(string $event): ObserverList {
        if ($this->hasObservers($event)) {
            return $this->observers[$event];
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
            usort($observers, ($a, $b) ==> {
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
        return $this->observers->contains($event);
    }

    /**
     * Subscribe multiple observers to multiple events through a listener object.
     *
     * @param \Titon\Event\Listener $listener
     * @return $this
     */
    public function listen(Listener $listener): this {
        foreach ($listener->subscribeToEvents() as $event => $options) {
            foreach ($this->parseListenerMap($options) as $opt) {
                // UNSAFE
                // Since inst_meth() requires literal strings and we are passing variables
                $this->subscribe($event, inst_meth($listener, $opt['method']), $opt['priority'], $opt['once']);
            }
        }

        return $this;
    }

    /**
     * Subscribe a callback (observer) to an event.
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
            $this->observers[$event] = Vector {};
        }

        if ($priority === self::AUTO_PRIORITY) {
            $priority = count($this->observers[$event]) + self::DEFAULT_PRIORITY;
        }

        $this->observers[$event][] = new Observer($callback, $priority, $once);

        return $this;
    }

    /**
     * Remove multiple observers from multiple events through a listener object.
     *
     * @param \Titon\Event\Listener $listener
     * @return $this
     */
    public function unlisten(Listener $listener): this {
        foreach ($listener->subscribeToEvents() as $event => $options) {
            foreach ($this->parseListenerMap($options) as $opt) {
                // UNSAFE
                // Since inst_meth() requires literal strings and we are passing variables
                $this->unsubscribe($event, inst_meth($listener, $opt['method']));
            }
        }

        return $this;
    }

    /**
     * Remove an observer from an event.
     *
     * @param string $event
     * @param \Titon\Event\ObserverCallback $callback
     * @return $this
     */
    public function unsubscribe(string $event, ObserverCallback $callback): this {
        $indices = Vector {};

        foreach ($this->getObservers($event) as $i => $observer) {
            if ($observer->getCallback() === $callback) {
                $indices[] = $i;
            }
        }

        // We must do this as you can't remove keys while iterating
        foreach ($indices as $i) {
            $this->observers[$event]->removeKey($i);
        }

        return $this;
    }

    /**
     * Notify the observer by executing the callback with the defined params.
     * Can optionally stop the event and set a state based on the callbacks response.
     *
     * @param \Titon\Event\Observer $observer
     * @param \Titon\Event\Event $event
     * @return bool
     */
    protected function executeObserver(Observer $observer, Event $event): bool {
        if ($event->isStopped()) {
            return false;

        } else if ($observer->isOnce() && $observer->hasExecuted()) {
            return true;
        }

        return $this->handleExecution($event, $observer->execute($event));
    }

    /**
     * Asynchronously notify the observer.
     *
     * @param \Titon\Event\Observer $observer
     * @param \Titon\Event\Event $event
     * @return bool
     */
    protected async function executeObserverAsync(Observer $observer, Event $event): Awaitable<bool> {
        if ($event->isStopped()) {
            return false;

        } else if ($observer->isOnce() && $observer->hasExecuted()) {
            return true;
        }

        $response = await $observer->asyncExecute($event);

        return $this->handleExecution($event, $response);
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
    protected function handleExecution(Event $event, mixed $response): bool {
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
     * @return bool
     */
    protected function notifyObservers(ObserverList $observers, Event $event): bool {
        foreach ($observers as $observer) {
            $this->executeObserver($observer, $event);

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
     * @return Awaitable<mixed>
     */
    protected async function notifyObserversAsync(ObserverList $observers, Event $event): Awaitable<mixed> {
        if ($event->isStopped()) {
            return false; // Exit early if non-async stopped propagation
        }

        $handles = Vector {};

        foreach ($observers as $observer) {
            $handles[] = $this->executeObserverAsync($observer, $event)->getWaitHandle();
        }

        await GenVectorWaitHandle::create($handles);

        return true;
    }

    /**
     * Parse the options from a listener into an indexed array of object method callbacks.
     *
     * @param mixed $options
     * @return Vector<ListenerOption>
     */
    protected function parseListenerMap(mixed $options): Vector<ListenerOption> {
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

}
