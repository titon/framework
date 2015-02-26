<?hh
namespace Titon\Test\Stub\Common;

use Countable;
use IteratorAggregate;
use Titon\Common\Mutable;

class MutableStub implements IteratorAggregate, Countable {
    use Mutable;
}
