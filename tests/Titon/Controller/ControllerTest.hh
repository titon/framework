<?hh
namespace Titon\Controller;

use Titon\Controller\Controller\AbstractController;
use Titon\Controller\Controller\ErrorController;
use Titon\Http\Exception\NotFoundException;
use Titon\Http\Server\Request;
use Titon\Http\Server\Response;
use Titon\Test\TestCase;
use Titon\View\View\Engine\TemplateEngine;
use Titon\View\View\TemplateView;
use VirtualFileSystem\FileSystem;

/**
 * @property \Titon\Controller\Controller $object
 */
class ControllerTest extends TestCase {

    protected function setUp() {
        parent::setUp();

        $this->vfs = new FileSystem();
        $this->vfs->createStructure([
            '/views/' => [
                'private/' => [
                    'errors/' => [
                        'error.tpl' => '<?php echo $message; ?>',
                        'http.tpl' => '<?php echo $code . \': \' . $message; ?>'
                    ],
                    'layouts/' => [
                        'default.tpl' => '<?php echo $this->getContent(); ?>'
                    ]
                ],
                'public/' => [
                    'core/' => [
                        'custom.tpl' => 'core:custom',
                        'index.tpl' => 'core:index'
                    ]
                ]
            ]
        ]);

        $this->object = new ControllerStub(Map {
            'module' => 'module',
            'controller' => 'controller',
            'action' => 'action',
            'args' => [100, 25]
        });
        $this->object->setRequest(Request::createFromGlobals());
        $this->object->setResponse(new Response());
    }

    protected function makeController() {
        $view = new TemplateView($this->vfs->path('/views/'));
        $view->setEngine(new TemplateEngine());

        $controller = new ErrorController(Map {'module' => 'main', 'controller' => 'core', 'action' => 'index'});
        $controller->setRequest(Request::createFromGlobals());
        $controller->setResponse(new Response());
        $controller->setView($view);
        $controller->initialize();

        return $controller;
    }

    public function testDispatchAction() {
        $this->assertEquals('actionNoArgs', $this->object->dispatchAction('action-no-args'));
        $this->assertEquals('actionNoArgs', $this->object->dispatchAction('actionNoArgs'));
        $this->assertEquals('actionNoArgs', $this->object->dispatchAction('actionNoArgs', ['foo', 'bar']));
        $this->assertEquals(125, $this->object->dispatchAction('actionWithArgs'));
        $this->assertEquals(555, $this->object->dispatchAction('actionWithArgs', [505, 50]));
        $this->assertEquals(335, $this->object->dispatchAction('actionWithArgs', [335]));
        $this->assertEquals(0, $this->object->dispatchAction('actionWithArgs', ['foo', 'bar']));
    }

    /**
     * @expectedException \Titon\Controller\Exception\InvalidActionException
     */
    public function testDispatchActionMissingAction() {
        $this->object->dispatchAction('noAction');
    }

    /**
     * @expectedException \Titon\Controller\Exception\InvalidActionException
     */
    public function testDispatchActionPrivateAction() {
        $this->object->dispatchAction('actionPrivate');
    }

    /**
     * @expectedException \Titon\Controller\Exception\InvalidActionException
     */
    public function testDispatchActionInheritedAction() {
        $this->object->dispatchAction('dispatchAction');
    }

    public function testForwardAction() {
        $this->object->forwardAction('actionNoArgs');
        $this->assertEquals('actionNoArgs', $this->object->getConfig('action'));

        $this->object->forwardAction('actionWithArgs');
        $this->assertEquals('actionWithArgs', $this->object->getConfig('action'));
    }

    public function testRenderView() {
        $controller = $this->makeController();

        // Using config
        $this->assertEquals('core:index', $controller->renderView());

        // Custom template
        $this->assertEquals('core:custom', $controller->renderView('core\custom'));

        // Custom template config
        $controller->setConfig('template', 'core/custom');
        $this->assertEquals('core:custom', $controller->renderView());
        $controller->setConfig('template', '');

        // Disable rendering
        $controller->setConfig('render', false);
        $this->assertEquals('', $controller->renderView());
    }

    public function testRenderError() {
        $controller = $this->makeController();

        $this->assertEquals('Message', $controller->renderError(new \Exception('Message')));
        $this->assertEquals(500, $controller->getResponse()->getStatusCode());

        error_reporting(0);

        $this->assertEquals('404: Not Found', $controller->renderError(new NotFoundException('Not Found')));
        $this->assertEquals(404, $controller->getResponse()->getStatusCode());

        error_reporting(E_ALL | E_STRICT);
    }

    public function testGetSetView() {
        $view = new TemplateView($this->vfs->path('/views/'));
        $this->assertEquals(null, $this->object->getView());

        $this->object->setView($view);
        $this->assertEquals($view, $this->object->getView());
    }

}

class ControllerStub extends AbstractController {

    public function actionWithArgs($arg1, $arg2 = 0): string {
        return strval($arg1 + $arg2);
    }

    public function actionNoArgs(): string {
        return 'actionNoArgs';
    }

    public function _actionPseudoPrivate(): string {
        return 'wontBeCalled';
    }

    protected function actionProtected(): string {
        return 'wontBeCalled';
    }

    private function actionPrivate(): string {
        return 'wontBeCalled';
    }

    public function renderView(mixed $template = ''): string {
    }

}