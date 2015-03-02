<?hh
namespace Titon\Event;

use Titon\Test\Stub\Event\CounterEventStub;
use Titon\Test\Stub\Event\ListenerStub;
use Titon\Test\Stub\Event\SubjectStub;
use Titon\Test\TestCase;

/**
 * @property \Titon\Test\Stub\Event\SubjectStub $object
 */
class SubjectTest extends TestCase {

    protected function setUp(): void {
        parent::setUp();

        $this->object = new SubjectStub();
    }

    public function testEmit(): void {
        $this->assertInstanceOf('Titon\Event\Event', $this->object->emit(new Event('event.test')));
    }

    public function testEmitMany(): void {
        $events = $this->object->emitMany([new Event('event.foo'), new Event('event.bar')]);

        $this->assertEquals(2, count($events));
        $this->assertInstanceOf('Titon\Event\Event', $events['event.foo']);
        $this->assertInstanceOf('Titon\Event\Event', $events['event.bar']);
    }

    public function testOnAndOff(): void {
        $callback = ($event) ==> { };
        $listener = new ListenerStub();

        $this->object->on('event.test1', $callback);
        $this->object->on($listener);

        $this->assertEquals(Vector {
            new Observer($callback, 100, false),
            new Observer(inst_meth($listener, 'noop1'), 101, false),
            new Observer(inst_meth($listener, 'noop2'), 45, false),
        }, $this->object->getEmitter()->getObservers('event.test1'));

        $this->object->off('event.test1', $callback);
        $this->object->off($listener);

        $this->assertEquals(Vector {}, $this->object->getEmitter()->getObservers('event.test1'));
    }

    public function testOnce(): void {
        $event = new CounterEventStub('event.test');

        $ob1 = function(CounterEventStub $event) { $event->count++; };
        $ob2 = [new ListenerStub(), 'counter'];
        $ob3 = function(CounterEventStub $event) { $event->count++; };
        $ob4 = [new ListenerStub(), 'counter'];

        $this->object->once('event.test', $ob1, 20);
        $this->object->on('event.test', $ob2, 15);
        $this->object->on('event.test', $ob3, 5, true);
        $this->object->on('event.test', $ob4, 75);

        $this->object->emit($event);

        $this->assertEquals(4, $event->count);

        $this->object->emit($event);

        $this->assertEquals(6, $event->count);
    }

}
