<?hh // strict
namespace Titon\Test\Stub\Event;

use Titon\Event\Event;

class CounterEventStub extends Event {
    public int $count = 0;
    public string $key = '';
}
