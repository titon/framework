<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Event;

use Titon\Event\Exception\InvalidObserverException;

/**
 * Provides functionality for the Subject while injecting a customizable Emitter.
 *
 * @package Titon\Event
 */
trait Emittable {
    require implements Subject;

    /**
     * Emitter object.
     *
     * @type \Titon\Event\Emitter
     */
    protected Emitter $_emitter;

    /**
     * @see \Titon\Event\Emitter::emit()
     */
    public function emit(string $event, ParamList $params = []): Event {
        return $this->getEmitter()->emit($event, $params);
    }

    /**
     * @see \Titon\Event\Emitter::emit()
     */
    public function emitMany(mixed $event, ParamList $params = []): EventMap {
        return $this->getEmitter()->emitMany($event, $params);
    }

    /**
     * Return the emitter. If it has not been defined, instantiate one.
     *
     * @return \Titon\Event\Emitter
     */
    public function getEmitter(): Emitter {
        if (!$this->_emitter) {
            $this->setEmitter(new Emitter());
        }

        return $this->_emitter;
    }

    /**
     * @see \Titon\Event\Subject::once()
     */
    public function once(string $event, mixed $callback, int $priority = 0): this {
        return $this->on($event, $callback, $priority, true);
    }

    /**
     * @see \Titon\Event\Subject::once()
     */
    public function on(string $event, mixed $callback, int $priority = 0, bool $once = false): this {
        if ($callback instanceof Listener) {
            $this->getEmitter()->registerListener($callback);

        } else if (is_callable($callback)) {
            // UNSAFE
            $this->getEmitter()->register($event, $callback, $priority, $once);

        } else {
            throw new InvalidObserverException('Observer must be a callable or a Listener');
        }

        return $this;
    }

    /**
     * @see \Titon\Event\Subject::once()
     */
    public function off(string $event, mixed $callback): this {
        if ($callback instanceof Listener) {
            $this->getEmitter()->removeListener($callback);

        } else if (is_callable($callback)) {
            // UNSAFE
            $this->getEmitter()->remove($event, $callback);

        } else {
            throw new InvalidObserverException('Observer must be a callable or a Listener');
        }

        return $this;
    }

    /**
     * Set the emitter to use.
     *
     * @param \Titon\Event\Emitter $emitter
     * @return $this
     */
    public function setEmitter(Emitter $emitter): this {
        $this->_emitter = $emitter;

        return $this;
    }

}