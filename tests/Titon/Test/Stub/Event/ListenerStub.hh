<?hh // strict
namespace Titon\Test\Stub\Event;

use Titon\Event\Event;
use Titon\Event\Listener;
use Titon\Event\ListenerMap;

class ListenerStub implements Listener {

    public function subscribeToEvents(): ListenerMap {
        return Map {
            'event.test1' => Vector {
                'noop1',
                Map {'method' => 'noop2', 'priority' => 45}
            },
            'event.test2' => 'noop1',
            'event.test3' => Map {'method' => 'noop2', 'priority' => 15}
        };
    }

    public function noop1(Event $event): void {}

    public function noop2(Event $event): void {}

    public function noop3(CounterEventStub $event): void {
        $event->key = 'bar';
    }

    public function counter(CounterEventStub $event): void {
        $event->count++;
    }

    public async function asyncNoop1(ListEventStub $event): Awaitable<mixed> {
        await SleepWaitHandle::create(rand(100, 1000) * 1000);

        $event->list[] = 1;

        return true;
    }

    public async function asyncNoop2(ListEventStub $event): Awaitable<mixed> {
        await SleepWaitHandle::create(rand(100, 500) * 1000);

        $event->list[] = 2;

        return true;
    }

    public async function asyncNoop3(ListEventStub $event): Awaitable<mixed> {
        await SleepWaitHandle::create(rand(1, 500) * 1000);

        $event->list[] = 3;

        return true;
    }

}
