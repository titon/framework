<?hh // strict
namespace Titon\Annotation;

use Titon\Test\Stub\Annotation\WireableStub;
use Titon\Test\TestCase;

class WireableTest extends TestCase {

    protected function setUp(): void {
        parent::setUp();

        Registry::map('Wire', 'Titon\Test\Stub\Annotation\WireAnnotationStub');
    }

    public function testWireAnnotations(): void {
        $stub = new WireableStub();

        $this->assertEquals('This is a class annotation', $stub->class);
        $this->assertEquals(Map {
            'doAction' => 'This is a method annotation'
        }, $stub->method);
    }

    public function testWireClassAnnotation(): void {
        $stub = new WireableStub('class');

        $this->assertEquals('This is a class annotation', $stub->class);
        $this->assertEquals(Map {}, $stub->method);
    }

    public function testWireMethodAnnotation(): void {
        $stub = new WireableStub('method');

        $this->assertEquals('', $stub->class);
        $this->assertEquals(Map {
            'doAction' => 'This is a method annotation'
        }, $stub->method);
    }

}
