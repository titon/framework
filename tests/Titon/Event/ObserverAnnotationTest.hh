<?hh
/* namespace Titon\Event;

use Titon\Common\Annotateable;
use Titon\Common\Annotator;
use Titon\Test\TestCase;

class ObserverAnnotationTest extends TestCase {

    public function testAnnotationWiring() {
        $stub = new ObserverAnnotationStub();

        $this->assertEquals(Vector {
            new Observer(inst_meth($stub, 'defaultObserver'), 100, false),
        }, $stub->getEmitter()->getObservers('event.foo'));

        $this->assertEquals(Vector {
            new Observer(inst_meth($stub, 'priorityObserver'), 5, false),
        }, $stub->getEmitter()->getObservers('event.bar'));

        $this->assertEquals(Vector {
            new Observer(inst_meth($stub, 'onceObserver'), 100, true),
        }, $stub->getEmitter()->getObservers('event.baz'));

        $this->assertEquals(Vector {
            new Observer(inst_meth($stub, 'asyncObserver'), 100, false),
        }, $stub->getEmitter()->getObservers('event.qux'));

        $this->assertTrue($stub->getEmitter()->getObservers('event.qux')[0]->isAsync());
    }

}

class ObserverAnnotationStub implements Annotator, Subject {
    use Annotateable, EmitsEvents, ObserverAnnotation;

    public function __construct() {
        $this->wireObserverAnnotations();
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
*/
