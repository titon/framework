<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Event;

/**
 * The Subject interface permits a class to manage observers and listeners, and emit events through an emitter.
 *
 * @package Titon\Event
 */
interface Subject {

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
    public function emit(string $event, array<mixed> $params = []): Event;

    /**
     * Emit multiple events at once by passing a list of event names, or event names separated by a space.
     * If a `*` is provided in the key, a wildcard match will occur.
     *
     * @param mixed $event
     * @param array<mixed> $params
     * @return \Titon\Event\EventMap
     */
    public function emitMany(mixed $event, array<mixed> $params = []): EventMap;

    /**
     * Register an event callback or listener to only trigger once and then remove itself from the observer list.
     *
     * @param string $event
     * @param mixed $callback
     * @param int $priority
     * @return $this
     */
    public function once(string $event, mixed $callback, int $priority = 0): this;

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
    public function on(string $event, mixed $callback, int $priority = 0, bool $once = false): this;

    /**
     * Remove an event callback or a listener.
     *
     * @param string $event
     * @param mixed $callback
     * @return $this
     * @throws \Titon\Event\Exception\InvalidObserverException
     */
    public function off(string $event, mixed $callback): this;

}