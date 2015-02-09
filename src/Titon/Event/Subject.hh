<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Event;

use Titon\Common\ArgumentList;

/**
 * The Subject interface permits a class to manage observers and listeners, and emit events through an emitter.
 *
 * @package Titon\Event
 */
interface Subject {

    /**
     * @see \Titon\Event\Emitter::emit()
     */
    public function emit(string $event, ArgumentList $args): Event;

    /**
     * @see \Titon\Event\Emitter::emitMany()
     */
    public function emitMany(mixed $event, ArgumentList $args): EventMap;

    /**
     * Return the emitter. If no emitter exists, automatically create one.
     *
     * @return \Titon\Event\Emitter
     */
    public function getEmitter(): Emitter;

    /**
     * Register an observer or listener to only trigger once.
     *
     * @param string $event
     * @param \Titon\Event\ObserverCallback $callback
     * @param int $priority
     * @return $this
     */
    public function once(string $event, ObserverCallback $callback, int $priority = Emitter::AUTO_PRIORITY): this;

    /**
     * Register an observer or a listener.
     *
     * @param mixed $event
     * @param \Titon\Event\ObserverCallback $callback
     * @param int $priority
     * @param bool $once
     * @return $this
     * @throws \Titon\Event\Exception\InvalidObserverException
     */
    public function on(mixed $event, ?ObserverCallback $callback = null, int $priority = Emitter::AUTO_PRIORITY, bool $once = false): this;

    /**
     * Remove an observer or a listener.
     *
     * @param mixed $event
     * @param \Titon\Event\ObserverCallback $callback
     * @return $this
     * @throws \Titon\Event\Exception\InvalidObserverException
     */
    public function off(mixed $event, ?ObserverCallback $callback = null): this;

    /**
     * Set an emitter.
     *
     * @param \Titon\Event\Emitter $emitter
     * @return $this
     */
    public function setEmitter(Emitter $emitter): this;

}
