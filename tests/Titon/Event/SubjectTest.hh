<?hh
namespace Titon\Event;

use Titon\Test\TestCase;

/**
 * @property \Titon\Event\SubjectStub $object
 */
class SubjectTest extends TestCase {

    protected function setUp() {
        parent::setUp();

        $this->object = new SubjectStub();
    }

    public function testEmit() {
        $this->assertInstanceOf('Titon\Event\Event', $this->object->emit('event.test', []));
    }

    public function testEmitMany() {
        $events = $this->object->emitMany('event.foo event.bar', []);

        $this->assertEquals(2, count($events));
        $this->assertInstanceOf('Titon\Event\Event', $events['event.foo']);
        $this->assertInstanceOf('Titon\Event\Event', $events['event.bar']);
    }

    public function testOnAndOff() {
        $callback = function(Event $event) { };
        $listener = new ListenerStub();

        $this->object->on('event.test1', $callback);
        $this->object->on('event', $listener);

        $this->assertEquals(Vector {
            new Observer($callback, 100, false),
            new Observer(inst_meth($listener, 'noop1'), 101, false),
            new Observer(inst_meth($listener, 'noop2'), 45, false),
        }, $this->object->getEmitter()->getObservers('event.test1'));

        $this->object->off('event.test1', $callback);
        $this->object->off('event', $listener);

        $this->assertEquals(Vector {}, $this->object->getEmitter()->getObservers('event.test1'));
    }

    public function testOnce() {
        $ob1 = function(Event $event) { };
        $ob2 = [new ListenerStub(), 'noop2'];
        $ob3 = function(Event $event) { };
        $ob4 = [new ListenerStub(), 'noop1'];

        $this->object->once('event.test', $ob1, 20);
        $this->object->on('event.test', $ob2, 15);
        $this->object->on('event.test', $ob3, 5, true);
        $this->object->on('event.test', $ob4, 75);

        $this->assertEquals(4, count($this->object->emit('event.test', [])->getCallStack()));
        $this->assertEquals(2, count($this->object->emit('event.test', [])->getCallStack()));
    }

}

class SubjectStub implements Subject {
    use Emittable;
}