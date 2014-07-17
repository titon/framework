<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Event;

/**
 * Provides class level event management.
 *
 * @package Titon\Event
 */
trait Emittable {

    /**
     * Emitter object.
     *
     * @type \Titon\Event\Emitter
     */
    protected Emitter $_emitter;

    /**
     * @see \Titon\Event\Emitter::emit()
     */
    public function emit(mixed $event, array $params = []): mixed {
        return $this->getEmitter()->emit($event, $params);
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
     * @see \Titon\Event\Emitter::once()
     *
     * @return $this
     */
    public function once(string $event, mixed $callback, Map<string, mixed> $options = Map {}): this {
        $this->getEmitter()->once($event, $callback, $options);

        return $this;
    }

    /**
     * @see \Titon\Event\Emitter::on()
     *
     * @return $this
     */
    public function on(?string $event, mixed $callback, Map<string, mixed> $options = Map {}): this {
        $this->getEmitter()->on($event, $callback, $options);

        return $this;
    }

    /**
     * @see \Titon\Event\Emitter::off()
     *
     * @return $this
     */
    public function off(?string $event, mixed $callback): this {
        $this->getEmitter()->off($event, $callback);

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