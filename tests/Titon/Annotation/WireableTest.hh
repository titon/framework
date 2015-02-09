<?hh
namespace Titon\Annotation;

use Titon\Test\TestCase;

class WireableTest extends TestCase {

    protected function setUp() {
        parent::setUp();

        Registry::map('Wire', 'Titon\Annotation\WireAnnotation');
    }

    public function testWireAnnotations() {
        $stub = new WireableStub();

        $this->assertEquals('This is a class annotation', $stub->class);
        $this->assertEquals(Map {
            'doAction' => 'This is a method annotation'
        }, $stub->method);
    }

    public function testWireClassAnnotation() {
        $stub = new WireableStub('class');

        $this->assertEquals('This is a class annotation', $stub->class);
        $this->assertEquals(Map {}, $stub->method);
    }

    public function testWireMethodAnnotation() {
        $stub = new WireableStub('method');

        $this->assertEquals('', $stub->class);
        $this->assertEquals(Map {
            'doAction' => 'This is a method annotation'
        }, $stub->method);
    }

}

class WireAnnotation extends Annotation implements Wireable {
    public function __construct(public string $type, public string $message) {}

    public function wire<T>(T $class, string $method = ''): this {
        if ($method) {
            $class->{$this->type}[$method] = $this->message;
        } else {
            $class->{$this->type} = $this->message;
        }
    }
}


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
