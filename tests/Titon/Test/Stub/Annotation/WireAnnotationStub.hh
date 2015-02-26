<?hh // strict
namespace Titon\Test\Stub\Annotation;

use Titon\Annotation\Annotation;
use Titon\Annotation\Wireable;

class WireAnnotationStub extends Annotation implements Wireable {
    public function __construct(public string $type, public string $message) {}

    public function wire<T>(T $class, string $method = ''): this {
        // UNSAFE
        if ($method) {
            $class->{$this->type}[$method] = $this->message;
        } else {
            $class->{$this->type} = $this->message;
        }

        return $this;
    }
}
