<?hh
namespace Titon\Event;

use Titon\Test\Stub\Event\CounterEventStub;
use Titon\Test\Stub\Event\ListenerStub;
use Titon\Test\Stub\Event\ListEventStub;
use Titon\Test\TestCase;

/**
 * @property \Titon\Event\Emitter $object
 */
class EmitterTest extends TestCase {

    protected function setUp(): void {
        parent::setUp();

        $this->object = new Emitter();
    }

    public function testFlush(): void {
        $ob1 = ($event) ==> { };
        $ob2 = ($event) ==> { };

        $this->object->subscribe('event.test1', $ob1);
        $this->object->subscribe('event.test2', $ob2);

        $this->assertEquals(Vector {'event.test1', 'event.test2'}, $this->object->getEventKeys());

        $this->object->flush('event.test2');
        $this->assertEquals(Vector {'event.test1'}, $this->object->getEventKeys());

        $this->object->flush();
        $this->assertEquals(Vector {}, $this->object->getEventKeys());
    }

    public function testGetObservers(): void {
        $ob1 = ($event) ==> { };
        $ob2 = [new ListenerStub(), 'noop1'];

        $this->object->subscribe('event.test', $ob1, 20);
        $this->object->subscribe('event.test', $ob2, 15);

        $this->assertEquals(Vector {}, $this->object->getObservers('event.foobar'));
        $this->assertEquals(Vector {
            new Observer($ob1, 20, false),
            new Observer($ob2, 15, false),
        }, $this->object->getObservers('event.test'));
    }

    public function testGetCallStack(): void {
        $ob1 = ($event) ==> { };
        $ob2 = [new ListenerStub(), 'noop2'];
        $ob3 = ($event) ==> { };
        $ob4 = [new ListenerStub(), 'noop3'];

        $this->object->subscribe('event.test', $ob1, 20);
        $this->object->subscribe('event.test', $ob2, 25);
        $this->object->subscribe('event.test', $ob3, 5);
        $this->object->subscribe('event.test', $ob4, 75);

        $this->assertEquals(Vector {
            '{closure}',
            '{closure}',
            'Titon\Test\Stub\Event\ListenerStub::noop2',
            'Titon\Test\Stub\Event\ListenerStub::noop3'
        }, $this->object->getCallStack('event.test'));
    }

    public function testGetEventKeys(): void {
        $this->assertEquals(Vector {}, $this->object->getEventKeys());

        $this->object->subscribe('event.test1', ($event) ==> { });
        $this->object->subscribe('event.test2', ($event) ==> { });

        $this->assertEquals(Vector {'event.test1', 'event.test2'}, $this->object->getEventKeys());
    }

    public function testGetSortedObservers(): void {
        $ob1 = ($event) ==> { };
        $ob2 = [new ListenerStub(), 'noop1'];
        $ob3 = ($event) ==> { };

        $this->object->subscribe('event.test', $ob1, 20);
        $this->object->subscribe('event.test', $ob2, 15);
        $this->object->subscribe('event.test', $ob3, 20);

        $this->assertEquals(Vector {}, $this->object->getObservers('event.foobar'));
        $this->assertEquals(Vector {
            new Observer($ob1, 20, false),
            new Observer($ob2, 15, false),
            new Observer($ob3, 20, false),
        }, $this->object->getObservers('event.test'));

        // Sorted
        $this->assertEquals(Vector {
            new Observer($ob2, 15, false),
            new Observer($ob1, 20, false),
            new Observer($ob3, 20, false),
        }, $this->object->getSortedObservers('event.test'));
    }

    public function testHasObservers(): void {
        $this->assertFalse($this->object->hasObservers('event.test'));

        $this->object->subscribe('event.test', ($event) ==> { });

        $this->assertTrue($this->object->hasObservers('event.test'));
    }

    public function testAsyncGetObservers(): void {
        $stub = new ListenerStub();

        $this->object->subscribe('event.test', [$stub, 'noop1']);
        $this->object->subscribe('event.test', [$stub, 'asyncNoop1']);

        $observers = $this->object->getObservers('event.test');

        $this->assertEquals(2, count($observers));
        $this->assertFalse($observers[0]->isAsync());
        $this->assertTrue($observers[1]->isAsync());
    }

    public function testExit(): void {
        $event = new CounterEventStub('event.test');

        $ob1 = ($event) ==> { $event->count++; };
        $ob2 = ($event) ==> { $event->count++; return true; };
        $ob3 = ($event) ==> { $event->count++; return false; };
        $ob4 = ($event) ==> { $event->count++; };

        $this->object->subscribe('event.test', $ob1);
        $this->object->subscribe('event.test', $ob2);
        $this->object->subscribe('event.test', $ob3);
        $this->object->subscribe('event.test', $ob4);

        $this->object->emit($event);

        $this->assertEquals(3, $event->count);
    }

    public function testData(): void {
        $ob1 = ($event) ==> { $event->setData('key', 1); };
        $ob2 = ($event) ==> { $event->setData('key', $event->getData('key') + 1); };
        $ob3 = ($event) ==> { $event->setData('key', $event->getData('key') + 1); };
        $ob4 = ($event) ==> { $event->setData('key', $event->getData('key') + 1); };

        $this->object->subscribe('event.test', $ob1);
        $this->object->subscribe('event.test', $ob2);
        $this->object->subscribe('event.test', $ob3);
        $this->object->subscribe('event.test', $ob4);

        $event = $this->object->emit(new Event('event.test'));

        $this->assertEquals(4, $event->getData('key'));
        $this->assertEquals(Map {'key' => 4}, $event->getData());
    }

    public function testSubscribeAndUnsubscribe(): void {
        $ob1 = ($event) ==> { };
        $ob2 = ($event) ==> { };

        $this->object->subscribe('event.test', $ob1);
        $this->object->subscribe('event.test', $ob2);

        $this->assertEquals(Vector {
            new Observer($ob1, 100, false),
            new Observer($ob2, 101, false)
        }, $this->object->getObservers('event.test'));

        // Remove using the instance
        $this->object->unsubscribe('event.test', $ob1);

        $this->assertEquals(Vector {
            new Observer($ob2, 101, false)
        }, $this->object->getObservers('event.test'));
    }

    public function testListenAndUnlisten(): void {
        $listener = new ListenerStub();

        $this->object->listen($listener);

        $this->assertEquals(Vector {
            'Titon\Test\Stub\Event\ListenerStub::noop2',
            'Titon\Test\Stub\Event\ListenerStub::noop1'
        }, $this->object->getCallStack('event.test1'));

        // Remove using the instance
        $this->assertEquals(Vector {'event.test1', 'event.test2', 'event.test3'}, $this->object->getEventKeys());

        $this->object->unlisten($listener);

        $this->assertEquals(Vector {}, $this->object->getCallStack('event.test1'));
    }

    public function testEmit(): void {
        $ob1 = ($event) ==> { };
        $ob2 = ($event) ==> { $event->stop(); };
        $ob3 = new ListenerStub();

        $this->object->subscribe('event.test', $ob1);
        $this->object->subscribe('event.test', $ob2);
        $this->object->subscribe('event.test', [$ob3, 'noop3']);

        $event = $this->object->emit(new Event('event.test'));

        $this->assertTrue($event->isStopped());
        $this->assertEquals(1, $event->getIndex());
    }

    public function testEmitNoObservers(): void {
        $event = $this->object->emit(new Event('event.fake'));

        $this->assertInstanceOf('Titon\Event\Event', $event);
        $this->assertEquals(Vector {}, $event->getCallStack());
    }

    public function testEmitParams(): void {
        $event = new CounterEventStub('event.test');

        $ob1 = new ListenerStub();
        $ob2 = ($event) ==> { $event->count++; };
        $ob3 = ($event) ==> { $event->count += 5; };

        $this->object->subscribe('event.test', [$ob1, 'noop3']);
        $this->object->subscribe('event.test', $ob2);
        $this->object->subscribe('event.test', $ob3);

        $this->object->emit($event);

        $this->assertEquals('bar', $event->key);
        $this->assertEquals(6, $event->count);
    }

    public function testEmitReturnState(): void {
        $event = new CounterEventStub('event.test');

        $ob1 = ($event) ==> { $event->count++; }; // Void, will continue
        $ob2 = ($event) ==> { $event->count++; return true; }; // True, will continue
        $ob3 = ($event) ==> { $event->count++; return ['foo' => 'bar']; }; // Has a value, will stop
        $ob4 = ($event) ==> { $event->count++; return 'foobar'; }; // Will not be reached

        $this->object->subscribe('event.test', $ob1);
        $this->object->subscribe('event.test', $ob2);
        $this->object->subscribe('event.test', $ob3);
        $this->object->subscribe('event.test', $ob4);

        $this->object->emit($event);

        $this->assertEquals(3, $event->count);
        $this->assertEquals(['foo' => 'bar'], $event->getState());
    }

    public function testEmitMany(): void {
        $ob1 = ($event) ==> { };
        $ob2 = ($event) ==> { $event->stop(); };
        $ob3 = new ListenerStub();

        $this->object->subscribe('event.test', $ob1);
        $this->object->subscribe('event.test', $ob2);
        $this->object->listen($ob3);

        $events = $this->object->emitMany(Vector {new Event('event.test'), new Event('event.test.2')});

        $this->assertEquals(2, count($events));
    }

    public function testEmitAsyncs(): void {
        $event = new ListEventStub('event.test');
        $stub = new ListenerStub();

        $this->object->subscribe('event.test', [$stub, 'asyncNoop1']);
        $this->object->subscribe('event.test', [$stub, 'asyncNoop2']);
        $this->object->subscribe('event.test', [$stub, 'asyncNoop3']);
        $this->object->subscribe('event.test', [$stub, 'asyncNoop1']);
        $this->object->subscribe('event.test', [$stub, 'asyncNoop2']);
        $this->object->subscribe('event.test', [$stub, 'asyncNoop3']);

        $this->assertEquals(6, count($this->object->getObservers('event.test')));

        $this->object->emit($event);

        $this->assertEquals(6, count($event->list));
        $this->assertNotEquals([1, 2, 3, 1, 2, 3], $event->list);
    }

}
