<?hh
namespace Titon\Test\Stub\Annotation;

use Titon\Annotation\WiresAnnotations;

<<Wire('class', 'This is a class annotation')>>
class WireableStub {
    use WiresAnnotations;

    public string $class = '';

    public Map<string, string> $method = Map {};

    public function __construct(string $type = 'both') {
        if ($type === 'method') {
            $this->wireMethodAnnotation('doAction', 'Wire');
        } else if ($type === 'class') {
            $this->wireClassAnnotation('Wire');
        } else {
            $this->wireAnnotations();
        }
    }

    <<Wire('method', 'This is a method annotation')>>
    public function doAction(): void {}
}
