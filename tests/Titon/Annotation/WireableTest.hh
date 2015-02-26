<?hh
namespace Titon\Annotation;

use Titon\Test\Stub\Annotation\WireableStub;
use Titon\Test\TestCase;

class WireableTest extends TestCase {

    protected function setUp() {
        parent::setUp();

        Registry::map('Wire', 'Titon\Test\Stub\Annotation\WireAnnotationStub');
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
