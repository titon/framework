<?hh // strict
namespace Titon\Test\Stub\Event;

use Titon\Event\Event;

class ListEventStub extends Event {
    public array<int> $list = [];
}
