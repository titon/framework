<?hh
/* namespace Titon\Route;

use Titon\Common\Annotateable;
use Titon\Common\Annotator;
use Titon\Test\TestCase;

class RouteAnnotationTest extends TestCase {

    public function testAnnotationWiring() {
        $router = Router::registry();

        $this->assertEquals(Map {}, $router->getRoutes());

        $stub = new RouteAnnotationStub();

        $this->assertEquals(Map {
            'routeannotationstub.list' => (new Route('/controller', 'Titon\Route\RouteAnnotationStub@index'))->setMethods(Vector {'get'}),
            'routeannotationstub.create' => (new Route('/controller', 'Titon\Route\RouteAnnotationStub@create'))->setMethods(Vector {'post'}),
            'routeannotationstub.read' => (new Route('/controller/{id}', 'Titon\Route\RouteAnnotationStub@read'))->setMethods(Vector {'get'}),
            'routeannotationstub.update' => (new Route('/controller/{id}', 'Titon\Route\RouteAnnotationStub@update'))->setMethods(Vector {'put', 'post'}),
            'routeannotationstub.delete' => (new Route('/controller/{id}', 'Titon\Route\RouteAnnotationStub@delete'))->setMethods(Vector {'delete', 'post'}),
            'routeannotationstub.foo' => new Route('/foo', 'Titon\Route\RouteAnnotationStub@foo'),
            'routeannotationstub.bar' => (new Route('/bar', 'Titon\Route\RouteAnnotationStub@bar'))->setMethods(Vector {'post'}),
            'routeannotationstub.baz' => (new Route('/baz', 'Titon\Route\RouteAnnotationStub@baz'))->setMethods(Vector {'get'})->setFilters(Vector {'auth', 'guest'})
        }, $router->getRoutes());
    }

}

<<Route('/controller')>>
class RouteAnnotationStub implements Annotator {
    use Annotateable, RouteAnnotation;

    public function __construct() {
        $this->wireRouteAnnotations();
    }

    <<Route('/foo')>>
    public function foo(): string {}

    <<Route('/bar', 'POST')>>
    public function bar(): string {}

    <<Route('/baz', 'get', ['auth', 'guest'])>>
    public function baz(): string {}
} */
