<?hh // strict
namespace Titon\Test\Stub\Event;

use Titon\Event\EmitsEvents;
use Titon\Event\Subject;

class SubjectStub implements Subject {
    use EmitsEvents;
}
