<?hh
namespace Titon\Test\Stub\Annotation;

use Titon\Annotation\Annotation;

class BazAnnotationStub extends Annotation {
    public function __construct(public array<mixed, mixed> $array) {}
}
