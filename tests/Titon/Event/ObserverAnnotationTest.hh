<?hh
namespace Titon\Event;

use Titon\Common\Annotateable;
use Titon\Common\Annotator;
use Titon\Test\TestCase;

class ObserverAnnotationTest extends TestCase {

    public function testAnnotationWiring() {
        $stub = new ObserverAnnotationStub();

        $this->assertEquals(Vector {
            shape('callback' => inst_meth($stub, 'defaultObserver'), 'priority' => 100, 'once' => false),
        }, $stub->getEmitter()->getObservers('event.foo'));

        $this->assertEquals(Vector {
            shape('callback' => inst_meth($stub, 'priorityObserver'), 'priority' => 5, 'once' => false),
        }, $stub->getEmitter()->getObservers('event.bar'));

        $this->assertEquals(Vector {
            shape('callback' => inst_meth($stub, 'onceObserver'), 'priority' => 100, 'once' => true),
        }, $stub->getEmitter()->getObservers('event.baz'));
    }

}

class ObserverAnnotationStub implements Annotator, Subject {
    use Annotateable, Emittable, ObserverAnnotation;

    public function __construct() {
        $this->__wireObserverAnnotations();
    }

    <<Observer('event.foo')>>
    public function defaultObserver(Event $event) {}

    <<Observer('event.bar', 5)>>
    public function priorityObserver(Event $event) {}

    <<Observer('event.baz', 0, true)>>
    public function onceObserver(Event $event) {}

}