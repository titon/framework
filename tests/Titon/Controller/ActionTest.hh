<?hh
namespace Titon\Controller;

use Titon\Controller\Action\AbstractAction;
use Titon\Controller\Controller\ErrorController;
use Titon\Http\Server\Request;
use Titon\Test\TestCase;

class ActionTest extends TestCase {

    public function testRun() {
        $controller = new ErrorController(Map {'foo' => 'bar'});
        $controller->setRequest(Request::createFromGlobals());

        $this->assertEquals('bar', $controller->getConfig('foo'));
        $this->assertFalse($controller->hasConfig('test'));

        $action = new ActionStub();
        $controller->runAction($action);

        $this->assertEquals($controller, $action->getController());

        $this->assertNotEquals('bar', $controller->getConfig('foo'));
        $this->assertEquals('baz', $controller->getConfig('foo'));
        $this->assertTrue($controller->hasConfig('test'));
    }

}

class ActionStub extends AbstractAction {

    public function get(): string {
        $this->getController()->addConfig(Map {
            'foo' => 'baz',
            'test' => 'value'
        });

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