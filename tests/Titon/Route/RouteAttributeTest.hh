<?hh
namespace Titon\Route;

use Titon\Common\Annotateable;
use Titon\Test\TestCase;

class RouteAttributeTest extends TestCase {

    public function testAnnotationWiring() {
        $router = Router::registry();

        $this->assertEquals(Map {}, $router->getRoutes());

        $stub = new RouteAttributeStub();

        $this->assertEquals(Map {
            'routeattributestub.list' => (new Route('/controller', 'Titon\Route\RouteAttributeStub@index'))->setMethods(Vector {'get'}),
            'routeattributestub.create' => (new Route('/controller', 'Titon\Route\RouteAttributeStub@create'))->setMethods(Vector {'post'}),
            'routeattributestub.read' => (new Route('/controller/{id}', 'Titon\Route\RouteAttributeStub@read'))->setMethods(Vector {'get'}),
            'routeattributestub.update' => (new Route('/controller/{id}', 'Titon\Route\RouteAttributeStub@update'))->setMethods(Vector {'put', 'post'}),
            'routeattributestub.delete' => (new Route('/controller/{id}', 'Titon\Route\RouteAttributeStub@delete'))->setMethods(Vector {'delete', 'post'}),
            'routeattributestub.foo' => new Route('/foo', 'Titon\Route\RouteAttributeStub@foo'),
            'routeattributestub.bar' => (new Route('/bar', 'Titon\Route\RouteAttributeStub@bar'))->setMethods(Vector {'post'}),
            'routeattributestub.baz' => (new Route('/baz', 'Titon\Route\RouteAttributeStub@baz'))->setMethods(Vector {'get'})->setFilters(Vector {'auth', 'guest'})
        }, $router->getRoutes());
    }

}

<<Route('/controller')>>
class RouteAttributeStub {
    use Annotateable, RouteAttribute;

    public function __construct() {
        $this->__wireRouteAnnotations();
    }

    <<Route('/foo')>>
    public function foo(): string {}

    <<Route('/bar', 'POST')>>
    public function bar(): string {}

    <<Route('/baz', 'get', ['auth', 'guest'])>>
    public function baz(): string {}
}