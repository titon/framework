<?hh
namespace Titon\Controller;

use Titon\Http\Server\Request;
use Titon\Test\Stub\Controller\ActionStub;
use Titon\Test\TestCase;
use Titon\Utility\State\Server;

class ActionTest extends TestCase {

    public function testRun(): void {
        $controller = new ErrorController();
        $controller->setRequest(Request::createFromGlobals());

        $this->assertObjectNotHasAttribute('foo', $controller);

        $action = new ActionStub();
        $controller->runAction($action);

        $this->assertEquals($controller, $action->getController());
        $this->assertObjectHasAttribute('foo', $controller);
        $this->assertEquals('bar', $controller->foo);
    }

    public function testRunWorksOnOtherHttpMethods(): void {
        $_SERVER['REQUEST_METHOD'] = 'HEAD';
        Server::initialize($_SERVER);

        $controller = new ErrorController();
        $controller->setRequest(Request::createFromGlobals());
        $controller->runAction(new ActionStub());

        $this->assertEquals('baz', $controller->foo);
    }

}
