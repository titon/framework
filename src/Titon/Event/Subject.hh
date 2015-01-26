<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
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
     * @see \Titon\Event\Emitter::emit()
     */
    public function emit(string $event, ParamList $params): Event;

    /**
     * @see \Titon\Event\Emitter::emitMany()
     */
    public function emitMany(mixed $event, ParamList $params): EventMap;

    /**
     * Register an observer or listener to only trigger once and then remove itself from the list.
     *
     * @param string $event
     * @param mixed $callback
     * @param int $priority
     * @return $this
     */
    public function once(string $event, mixed $callback, int $priority = Emitter::AUTO_PRIORITY): this;

    /**
     * Register an observer or a listener.
     *
     * @param string $event
     * @param mixed $callback
     * @param int $priority
     * @param bool $once
     * @return $this
     * @throws \Titon\Event\Exception\InvalidObserverException
     */
    public function on(string $event, mixed $callback, int $priority = Emitter::AUTO_PRIORITY, bool $once = false): this;

    /**
     * Remove an observer or a listener.
     *
     * @param string $event
     * @param mixed $callback
     * @return $this
     * @throws \Titon\Event\Exception\InvalidObserverException
     */
    public function off(string $event, mixed $callback): this;

}
