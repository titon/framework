<?hh // strict
namespace Titon\Test\Stub\Annotation;

use Titon\Annotation\Annotation;
use Titon\Annotation\Wireable;

class WireAnnotationStub extends Annotation implements Wireable {

    public string $message;

    public function __construct(string $message) {
        $this->message = $message;
    }

    public function wire<T>(T $class, string $method = ''): this {
        invariant($class instanceof WireableStub, 'Must be a WireableStub.');

        if ($method) {
            $class->method[$method] = $this->message;
        } else {
            $class->class = $this->message;
        }

        return $this;
    }
}
