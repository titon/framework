<?hh // strict
namespace Titon\Test\Stub\Event;

use Titon\Event\Event;
use Titon\Event\Listener;
use Titon\Event\ListenerMap;
use stdClass;

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

    public function noop1(Event $e): void {}

    public function noop2(Event $e): void {}

    public function noop3(Event $e, int $int, stdClass $object): void {
        $object->foo = 'bar';
    }

    public function counter(Event $e, int &$count): void {
        $count++;
    }

    public async function asyncNoop1(Event $e, array<int> &$list): Awaitable<mixed> {
        await SleepWaitHandle::create(rand(100, 1000) * 1000);

        $list[] = 1;

        return true;
    }

    public async function asyncNoop2(Event $e, array<int> &$list): Awaitable<mixed> {
        await SleepWaitHandle::create(rand(100, 500) * 1000);

        $list[] = 2;

        return true;
    }

    public async function asyncNoop3(Event $e, array<int> &$list): Awaitable<mixed> {
        await SleepWaitHandle::create(rand(1, 500) * 1000);

        $list[] = 3;

        return true;
    }

}
