<?hh // strict
namespace Titon\Test\Stub\Event;

use Titon\Annotation\WiresAnnotations;
use Titon\Event\EmitsEvents;
use Titon\Event\Event;
use Titon\Event\Subject;

class ObserverClassStub implements Subject {
    use EmitsEvents, WiresAnnotations;

    public function __construct(): void {
        $this->wireMethodAnnotation('defaultObserver', 'Observer');
        $this->wireMethodAnnotation('priorityObserver', 'Observer');
        $this->wireMethodAnnotation('onceObserver', 'Observer');
        $this->wireMethodAnnotation('asyncObserver', 'Observer');
    }

    <<Observer('event.foo')>>
    public function defaultObserver(Event $event): void {}

    <<Observer('event.bar', 5)>>
    public function priorityObserver(Event $event): void {}

    <<Observer('event.baz', 0, true)>>
    public function onceObserver(Event $event): void {}

    <<Observer('event.qux')>>
    public async function asyncObserver(Event $event): Awaitable<mixed> {}

}
