<?hh
namespace Titon\Event\Annotation;

use Titon\Annotation\WiresAnnotations;
use Titon\Event\EmitsEvents;
use Titon\Event\Observer as Ob;
use Titon\Event\Subject;
use Titon\Test\TestCase;

class ObserverTest extends TestCase {

    public function testObserversSubscribeToEvents() {
        $stub = new ObserverStub();

        $this->assertEquals(Vector {
            new Ob(inst_meth($stub, 'defaultObserver'), 100, false),
        }, $stub->getEmitter()->getObservers('event.foo'));

        $this->assertEquals(Vector {
            new Ob(inst_meth($stub, 'priorityObserver'), 5, false),
        }, $stub->getEmitter()->getObservers('event.bar'));

        $this->assertEquals(Vector {
            new Ob(inst_meth($stub, 'onceObserver'), 100, true),
        }, $stub->getEmitter()->getObservers('event.baz'));

        $this->assertEquals(Vector {
            new Ob(inst_meth($stub, 'asyncObserver'), 100, false),
        }, $stub->getEmitter()->getObservers('event.qux'));

        $this->assertTrue($stub->getEmitter()->getObservers('event.qux')[0]->isAsync());
    }

}

class ObserverStub implements Subject {
    use EmitsEvents, WiresAnnotations;

    public function __construct() {
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
