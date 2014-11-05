<?hh
namespace Titon\Controller;

use Titon\Controller\Action\AbstractAction;
use Titon\Http\Server\Request;
use Titon\Test\TestCase;

class ActionTest extends TestCase {

    public function testRun() {
        $controller = new ErrorController();
        $controller->setRequest(Request::createFromGlobals());

        $this->assertObjectNotHasAttribute('foo', $controller);

        $action = new ActionStub();
        $controller->runAction($action);

        $this->assertEquals($controller, $action->getController());
        $this->assertObjectHasAttribute('foo', $controller);
    }

}

class ActionStub extends AbstractAction {

    public function get(): string {
        $this->getController()->foo = 'bar';

        return 'get';
    }

    public function post(): string {
        return 'post';
    }

    public function delete(): string {
        return 'delete';
    }

    public function put(): string {
        return 'put';
    }

}