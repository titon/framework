<?hh
namespace Titon\Route\Annotation;

use Titon\Annotation\Reader;
use Titon\Test\TestCase;

class RouteTest extends TestCase {

    public function testParamsAreSetOnRouteAnnotation() {
        $reader = new Reader(new RouteStub());

        // Class
        $class = $reader->getClassAnnotation('Route');

        $this->assertEquals('parent', $class->getKey());
        $this->assertEquals('/controller', $class->getPath());
        $this->assertEquals(Vector {}, $class->getMethods());
        $this->assertEquals(Vector {}, $class->getFilters());
        $this->assertEquals(Map {}, $class->getPatterns());

        // Foo
        $foo = $reader->getMethodAnnotation('foo', 'Route');

        $this->assertEquals('foo', $foo->getKey());
        $this->assertEquals('/foo', $foo->getPath());
        $this->assertEquals(Vector {}, $foo->getMethods());
        $this->assertEquals(Vector {}, $foo->getFilters());
        $this->assertEquals(Map {}, $foo->getPatterns());

        // Bar
        $bar = $reader->getMethodAnnotation('bar', 'Route');

        $this->assertEquals('bar', $bar->getKey());
        $this->assertEquals('/bar', $bar->getPath());
        $this->assertEquals(Vector {'post'}, $bar->getMethods());
        $this->assertEquals(Vector {}, $bar->getFilters());
        $this->assertEquals(Map {}, $bar->getPatterns());

        // Baz
        $baz = $reader->getMethodAnnotation('baz', 'Route');

        $this->assertEquals('baz', $baz->getKey());
        $this->assertEquals('/baz', $baz->getPath());
        $this->assertEquals(Vector {'get'}, $baz->getMethods());
        $this->assertEquals(Vector {'auth', 'guest'}, $baz->getFilters());
        $this->assertEquals(Map {}, $baz->getPatterns());

        // Qux
        $qux = $reader->getMethodAnnotation('qux', 'Route');

        $this->assertEquals('qux', $qux->getKey());
        $this->assertEquals('/qux', $qux->getPath());
        $this->assertEquals(Vector {'put', 'post'}, $qux->getMethods());
        $this->assertEquals(Vector {}, $qux->getFilters());
        $this->assertEquals(Map {'id' => '[1-8]+'}, $qux->getPatterns());
    }

}

<<Route('parent', '/controller')>>
class RouteStub {

    <<Route('foo', '/foo')>>
    public function foo(): void {}

    <<Route('bar', '/bar', 'POST')>>
    public function bar(): void {}

    <<Route('baz', '/baz', ['get'], ['auth', 'guest'])>>
    public function baz(): void {}

    <<Route('qux', '/qux', ['PUT', 'POST'], [], ['id' => '[1-8]+'])>>
    public function qux(): void {}

}
