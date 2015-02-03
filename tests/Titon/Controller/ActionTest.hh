<?hh
namespace Titon\Controller;

use Titon\Controller\Action\AbstractAction;
use Titon\Http\Server\Request;
use Titon\Test\TestCase;
use Titon\Utility\State\Server;

class ActionTest extends TestCase {

    public function testRun() {
        $controller = new ErrorController();
        $controller->setRequest(Request::createFromGlobals());

        $this->assertObjectNotHasAttribute('foo', $controller);

        $action = new ActionStub();
        $controller->runAction($action);

        $this->assertEquals($controller, $action->getController());
        $this->assertObjectHasAttribute('foo', $controller);
        $this->assertEquals('bar', $controller->foo);
    }

    public function testRunWorksOnOtherHttpMethods() {
        $_SERVER['REQUEST_METHOD'] = 'HEAD';
        Server::initialize($_SERVER);

        $controller = new ErrorController();
        $controller->setRequest(Request::createFromGlobals());
        $controller->runAction(new ActionStub());

        $this->assertEquals('baz', $controller->foo);
    }

}

class ActionStub extends AbstractAction {

    public function get(): mixed {
        $this->getController()->foo = 'bar';

        return 'get';
    }

    public function post(): mixed {
        return 'post';
    }

    public function delete(): mixed {
        return 'delete';
    }

    public function put(): mixed {
        return 'put';
    }

    public function head(): mixed {
        $this->getController()->foo = 'baz';

        return 'head';
    }

}