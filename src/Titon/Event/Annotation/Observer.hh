<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Event\Annotation;

use Titon\Annotation\Annotation;
use Titon\Annotation\Wireable;
use Titon\Event\Emitter;
use Titon\Event\Subject;

/**
 * The Observer annotation will auto-subscribe class methods as event observers.
 * The parent class will need to implement the `Titon\Event\Subject` interface.
 *
 * <<Observer($event[, $priority[, $once]])>>
 *
 * @package Titon\Event\Annotation
 */
class Observer extends Annotation implements Wireable {

    /**
     * Name of the event to subscribe to.
     *
     * @var string
     */
    protected string $event;

    /**
     * Should the observer be executed once?
     *
     * @var bool
     */
    protected bool $once;

    /**
     * The priority level of the observer.
     *
     * @var int
     */
    protected int $priority;

    /**
     * Store relevant subscription settings.
     *
     * @param string $event
     * @param int    $priority
     * @param bool   $once
     */
    public function __construct(string $event, int $priority = Emitter::AUTO_PRIORITY, bool $once = false) {
        $this->event = $event;
        $this->priority = $priority;
        $this->once = $once;
    }

    /**
     * Return the event key.
     *
     * @return string
     */
    public function getEvent(): string {
        return $this->event;
    }

    /**
     * Return the priority number.
     *
     * @return int
     */
    public function getPriority(): int {
        return $this->priority;
    }

    /**
     * Return true if the observer should be executed once.
     *
     * @return bool
     */
    public function isOnce(): bool {
        return $this->once;
    }

    /**
     * Subscribe the observer to an event.
     *
     * @param T $class
     * @param string $method
     * @return $this
     */
    public function wire<T>(T $class, string $method = ''): this {
        if ($class instanceof Subject) {
            // UNSAFE
            // Since `inst_meth()` requires literal strings and we are passing variables
            $class->on($this->getEvent(), inst_meth($class, $method), $this->getPriority(), $this->isOnce());
        }

        return $this;
    }

}
