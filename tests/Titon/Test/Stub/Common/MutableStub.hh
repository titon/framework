<?hh // strict
namespace Titon\Test\Stub\Common;

use Countable;
use IteratorAggregate;
use Titon\Common\Mutable;

class MutableStub<Tk, Tv> implements IteratorAggregate<Tv>, Countable {
    use Mutable<Tk, Tv>;
}
