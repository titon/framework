<?hh // strict
namespace Titon\Test\Stub\Annotation;

use Titon\Annotation\Annotation;

class BarAnnotationStub extends Annotation {
    public function __construct(public string $string, public int $int = 0): void {}
}
