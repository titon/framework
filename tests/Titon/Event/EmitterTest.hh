<?hh
namespace Titon\Event;

use Titon\Test\TestCase;

/**
 * @property \Titon\Event\Emitter $object
 */
class EmitterTest extends TestCase {

    protected function setUp() {
        parent::setUp();

        $this->object = new Emitter();
    }

    public function testFlush() {
        $ob1 = function(Event $event) { };
        $ob2 = function(Event $event) { };

        $this->object->register('event.test1', $ob1);
        $this->object->register('event.test2', $ob2);

        $this->assertEquals(Vector {'event.test1', 'event.test2'}, $this->object->getEventKeys());

        $this->object->flush('event.test2');
        $this->assertEquals(Vector {'event.test1'}, $this->object->getEventKeys());

        $this->object->flush();
        $this->assertEquals(Vector {}, $this->object->getEventKeys());
    }

    public function testGetObservers() {
        $ob1 = function(Event $event) { };
        $ob2 = [new ListenerStub(), 'noop1'];

        $this->object->register('event.test', $ob1, 20);
        $this->object->register('event.test', $ob2, 15);

        $this->assertEquals(Vector {}, $this->object->getObservers('event.foobar'));
        $this->assertEquals(Vector {
            shape('callback' => $ob1, 'priority' => 20, 'once' => false),
            shape('callback' => $ob2, 'priority' => 15, 'once' => false),
        }, $this->object->getObservers('event.test'));
    }

    public function testGetCallStack() {
        $ob1 = function(Event $event) { };
        $ob2 = [new ListenerStub(), 'noop2'];
        $ob3 = function(Event $event) { };
        $ob4 = [new ListenerStub(), 'noop3'];

        $this->object->register('event.test', $ob1, 20);
        $this->object->register('event.test', $ob2, 15);
        $this->object->register('event.test', $ob3, 5);
        $this->object->register('event.test', $ob4, 75);

        $this->assertEquals(Vector {
            shape('priority' => 5, 'callback' => '{closure}', 'once' => false, 'time' => 0),
            shape('priority' => 15, 'callback' => 'Titon\Event\ListenerStub::noop2', 'once' => false, 'time' => 0),
            shape('priority' => 20, 'callback' => '{closure}', 'once' => false, 'time' => 0),
            shape('priority' => 75, 'callback' => 'Titon\Event\ListenerStub::noop3', 'once' => false, 'time' => 0)
        }, $this->object->getCallStack('event.test'));
    }

    public function testGetEventKeys() {
        $this->assertEquals(Vector {}, $this->object->getEventKeys());

        $this->object->register('event.test1', function(Event $event) { });
        $this->object->register('event.test2', function(Event $event) { });

        $this->assertEquals(Vector {'event.test1', 'event.test2'}, $this->object->getEventKeys());
    }

    public function testGetSortedObservers() {
        $ob1 = function(Event $event) { };
        $ob2 = [new ListenerStub(), 'noop1'];
        $ob3 = function(Event $event) { };

        $this->object->register('event.test', $ob1, 20);
        $this->object->register('event.test', $ob2, 15);
        $this->object->register('event.test', $ob3, 20);

        $this->assertEquals(Vector {}, $this->object->getObservers('event.foobar'));
        $this->assertEquals(Vector {
            shape('callback' => $ob1, 'priority' => 20, 'once' => false),
            shape('callback' => $ob2, 'priority' => 15, 'once' => false),
            shape('callback' => $ob3, 'priority' => 20, 'once' => false),
        }, $this->object->getObservers('event.test'));

        // Sorted
        $this->assertEquals(Vector {
            shape('callback' => $ob2, 'priority' => 15, 'once' => false),
            shape('callback' => $ob1, 'priority' => 20, 'once' => false),
            shape('callback' => $ob3, 'priority' => 20, 'once' => false),
        }, $this->object->getSortedObservers('event.test'));
    }

    public function testHasObservers() {
        $this->assertFalse($this->object->hasObservers('event.test'));
        $this->object->register('event.test', function(Event $event) { });
        $this->assertTrue($this->object->hasObservers('event.test'));
    }

    public function testOnAndOff() {
        $callback = function(Event $event) { };
        $listener = new ListenerStub();

        $this->object->on('event.test1', $callback);
        $this->object->on('event', $listener);

        $this->assertEquals(Vector {
            shape('callback' => $callback, 'priority' => 100, 'once' => false),
            shape('callback' => inst_meth($listener, 'noop1'), 'priority' => 101, 'once' => false),
            shape('callback' => inst_meth($listener, 'noop2'), 'priority' => 45, 'once' => false),
        }, $this->object->getObservers('event.test1'));

        $this->object->off('event.test1', $callback);
        $this->object->off('event', $listener);

        $this->assertEquals(Vector {}, $this->object->getObservers('event.test1'));
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

        $this->assertEquals(4, count($this->object->emit('event.test')->getCallStack()));
        $this->assertEquals(2, count($this->object->emit('event.test')->getCallStack()));
    }

    public function testExit() {
        $n = 0;
        $ob1 = function(Event $event, &$i) { $i++; };
        $ob2 = function(Event $event, &$i) { $i++; return true; };
        $ob3 = function(Event $event, &$i) { $i++; return false; };
        $ob4 = function(Event $event, &$i) { $i++; };

        $this->object->on('event.test', $ob1);
        $this->object->on('event.test', $ob2);
        $this->object->on('event.test', $ob3);
        $this->object->on('event.test', $ob4);

        $this->object->emit('event.test', [&$n]);

        $this->assertEquals(3, $n);
    }

    public function testData() {
        $ob1 = function(Event $event) { $event->setData('key', $event->getData('key') + 1); };
        $ob2 = function(Event $event) { $event->setData('key', $event->getData('key') + 1); };
        $ob3 = function(Event $event) { $event->setData('key', $event->getData('key') + 1); };
        $ob4 = function(Event $event) { $event->setData('key', $event->getData('key') + 1); };

        $this->object->on('event.test', $ob1);
        $this->object->on('event.test', $ob2);
        $this->object->on('event.test', $ob3);
        $this->object->on('event.test', $ob4);

        $event = $this->object->emit('event.test');

        $this->assertEquals(4, $event->getData('key'));
        $this->assertEquals(Map {'key' => 4}, $event->getData());
    }

    public function testRegisterAndRemove() {
        $ob1 = function(Event $event) { };
        $ob2 = function(Event $event) { };

        $this->object->register('event.test', $ob1);
        $this->object->register('event.test', $ob2);

        $this->assertEquals(Vector {
            shape('callback' => $ob1, 'priority' => 100, 'once' => false),
            shape('callback' => $ob2, 'priority' => 101, 'once' => false)
        }, $this->object->getObservers('event.test'));

        // Remove using the instance
        $this->object->remove('event.test', $ob1);

        $this->assertEquals(Vector {
            shape('callback' => $ob2, 'priority' => 101, 'once' => false)
        }, $this->object->getObservers('event.test'));
    }

    public function testRegisterAndRemoveListeners() {
        $listener = new ListenerStub();

        $this->object->registerListener($listener);

        $this->assertEquals(Vector {
            shape('callback' => 'Titon\Event\ListenerStub::noop2', 'priority' => 45, 'once' => false, 'time' => 0),
            shape('callback' => 'Titon\Event\ListenerStub::noop1', 'priority' => 100, 'once' => false, 'time' => 0),
        }, $this->object->getCallStack('event.test1'));

        // Remove using the instance
        $this->assertEquals(Vector {'event.test1', 'event.test2', 'event.test3'}, $this->object->getEventKeys());

        $this->object->removeListener($listener);

        $this->assertEquals(Vector {}, $this->object->getCallStack('event.test1'));
    }

    public function testEmit() {
        $ob1 = function(Event $event) { };
        $ob2 = function(Event $event) { $event->stop(); };
        $ob3 = new ListenerStub();

        $this->object->register('event.test', $ob1);
        $this->object->register('event.test', $ob2);
        $this->object->register('event.test', [$ob3, 'noop3']);

        $event = $this->object->emit('event.test');
        $this->assertTrue($event->isStopped());
        $this->assertEquals(1, $event->getIndex());
    }

    public function testEmitNoObservers() {
        $event = $this->object->emit('fake.event');
        $this->assertInstanceOf('Titon\Event\Event', $event);
        $this->assertEquals(Vector {}, $event->getCallStack());
    }

    public function testEmitParams() {
        $ob1 = new ListenerStub();
        $ob2 = function(Event $event, &$int, $object) { $int++; };
        $ob3 = function(Event $event, &$int, $object) { $int += 5; };

        $this->object->register('event.test', [$ob1, 'noop3']);
        $this->object->register('event.test', $ob2);
        $this->object->register('event.test', $ob3);

        $int = 0;
        $object = new \stdClass();
        $object->foo = 'baz';

        $this->object->emit('event.test', [&$int, $object]);

        $this->assertEquals('bar', $object->foo);
        $this->assertEquals(6, $int);
    }

    public function testEmitReturnState() {
        $count = 0;
        $ob1 = function(Event $event) use (&$count) { $count++; }; // Void, will continue
        $ob2 = function(Event $event) use (&$count) { $count++; return true; }; // True, will continue
        $ob3 = function(Event $event) use (&$count) { $count++; return ['foo' => 'bar']; }; // Has a value, will stop
        $ob4 = function(Event $event) use (&$count) { $count++; return 'foobar'; }; // Will not be reached

        $this->object->register('event.test', $ob1);
        $this->object->register('event.test', $ob2);
        $this->object->register('event.test', $ob3);
        $this->object->register('event.test', $ob4);

        $event = $this->object->emit('event.test');

        $this->assertEquals(3, $count);
        $this->assertEquals(['foo' => 'bar'], $event->getState());
    }

    public function testEmitMany() {
        $ob1 = function(Event $event) { };
        $ob2 = function(Event $event) { $event->stop(); };
        $ob3 = new ListenerStub();

        $this->object->register('event.test', $ob1);
        $this->object->register('event.test', $ob2);
        $this->object->registerListener($ob3);

        $events = $this->object->emitMany('event.test event.test1');
        $this->assertEquals(2, count($events));
    }

    public function testEmitManyWildcard() {
        $ob1 = function(Event $event) { };
        $ob2 = function(Event $event) { $event->stop(); };
        $ob3 = new ListenerStub();

        $this->object->register('event.cb1', $ob1);
        $this->object->register('event.cb2', $ob2);
        $this->object->registerListener($ob3);

        $events = $this->object->emitMany('event.*');
        $this->assertEquals(5, count($events));
    }

}

class ListenerStub implements Listener {

    public function registerEvents(): ListenerMap {
        return Map {
            'event.test1' => Vector {
                'noop1',
                Map {'method' => 'noop2', 'priority' => 45}
            },
            'event.test2' => 'noop1',
            'event.test3' => Map {'method' => 'noop2', 'priority' => 15}
        };
    }

    public function noop1(Event $e): void {}

    public function noop2(Event $e): void {}

    public function noop3(Event $e, $object1, $object2): void {
        $object2->foo = 'bar';
    }

}