<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
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
trait EmitsEvents {
    require implements Subject;

    /**
     * Emitter object.
     *
     * @var \Titon\Event\Emitter
     */
    protected ?Emitter $emitter;

    /**
     * @see \Titon\Event\Emitter::emit()
     */
    public function emit(string $event, ParamList $params): Event {
        return $this->getEmitter()->emit($event, $params);
    }

    /**
     * @see \Titon\Event\Emitter::emit()
     */
    public function emitMany(mixed $event, ParamList $params): EventMap {
        return $this->getEmitter()->emitMany($event, $params);
    }

    /**
     * @see \Titon\Event\Subject::getEmitter()
     */
    public function getEmitter(): Emitter {
        if (!$this->emitter) {
            $this->emitter = new Emitter();
        }

        return $this->emitter;
    }

    /**
     * @see \Titon\Event\Subject::once()
     */
    public function once(string $event, ObserverCallback $callback, int $priority = Emitter::AUTO_PRIORITY): this {
        return $this->on($event, $callback, $priority, true);
    }

    /**
     * @see \Titon\Event\Subject::once()
     */
    public function on(mixed $event, ?ObserverCallback $callback = null, int $priority = Emitter::AUTO_PRIORITY, bool $once = false): this {
        if ($event instanceof Listener) {
            $this->getEmitter()->listen($event);

        } else if ($callback !== null) {
            $this->getEmitter()->subscribe((string) $event, $callback, $priority, $once);

        } else {
            throw new InvalidObserverException('Observer must be a callable or a Listener');
        }

        return $this;
    }

    /**
     * @see \Titon\Event\Subject::once()
     */
    public function off(mixed $event, ?ObserverCallback $callback = null): this {
        if ($event instanceof Listener) {
            $this->getEmitter()->unlisten($event);

        } else if ($callback !== null) {
            $this->getEmitter()->unsubscribe((string) $event, $callback);

        } else {
            throw new InvalidObserverException('Observer must be a callable or a Listener');
        }

        return $this;
    }

    /**
     * @see \Titon\Event\Subject::setEmitter()
     */
    public function setEmitter(Emitter $emitter): this {
        $this->emitter = $emitter;

        return $this;
    }

}
