<?hh // strict
namespace Titon\Event\Annotation;

use Titon\Event\Observer;
use Titon\Test\Stub\Event\ObserverClassStub;
use Titon\Test\TestCase;

class ObserverTest extends TestCase {

    public function testObserversSubscribeToEvents(): void {
        $stub = new ObserverClassStub();

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
