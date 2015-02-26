<?hh
namespace Titon\Controller;

use Titon\Http\Exception\NotFoundException;
use Titon\Http\Server\Request;
use Titon\Http\Server\Response;
use Titon\Test\Stub\Controller\ControllerStub;
use Titon\Test\TestCase;
use Titon\View\Engine\TemplateEngine;
use Titon\View\EngineView;

/**
 * @property \Titon\Test\Stub\Controller\ControllerStub $object
 */
class ControllerTest extends TestCase {

    protected function setUp() {
        parent::setUp();

        $this->setupVFS();
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
                    'stub/' => [
                        'action-no-args.tpl' => 'stub:action-no-args',
                        'index.tpl' => 'stub:index'
                    ]
                ]
            ]
        ]);

        $view = new EngineView($this->vfs->path('/views/'));
        $view->setEngine(new TemplateEngine());

        $this->object = new ControllerStub();
        $this->object->setRequest(Request::createFromGlobals());
        $this->object->setResponse(new Response());
        $this->object->setView($view);
    }

    public function testBuildViewPath() {
        $this->assertEquals('stub/index', $this->object->buildViewPath('index'));
        $this->assertEquals('stub/action-no-args', $this->object->buildViewPath('actionNoArgs'));
        $this->assertEquals('stub/action-with-args', $this->object->buildViewPath('actionWithArgs'));
    }

    public function testDispatchTo() {
        $this->assertEquals('actionNoArgs', $this->object->dispatchTo('action-no-args', []));
        $this->assertEquals('actionNoArgs', $this->object->dispatchTo('actionNoArgs', []));
        $this->assertEquals('actionNoArgs', $this->object->dispatchTo('actionNoArgs', ['foo', 'bar']));
        $this->assertEquals(125, $this->object->dispatchTo('actionWithArgs', [125]));
        $this->assertEquals(555, $this->object->dispatchTo('actionWithArgs', [505, 50]));
        $this->assertEquals(335, $this->object->dispatchTo('actionWithArgs', [335]));
        $this->assertEquals(0, $this->object->dispatchTo('actionWithArgs', ['foo', 'bar']));
    }

    /**
     * @expectedException \Titon\Controller\Exception\InvalidActionException
     */
    public function testDispatchToMissingAction() {
        $this->object->dispatchTo('noAction', []);
    }

    public function testForwardTo() {
        $this->object->forwardTo('actionNoArgs', []);
        $this->assertEquals('actionNoArgs', $this->object->getCurrentAction());

        $this->object->forwardTo('actionWithArgs', ['foo']);
        $this->assertEquals('actionWithArgs', $this->object->getCurrentAction());
    }

    public function testGetActionAndArguments() {
        $this->object->dispatchTo('actionNoArgs', []);
        $this->assertEquals('actionNoArgs', $this->object->getCurrentAction());
        $this->assertEquals([], $this->object->getCurrentArguments());

        $this->object->dispatchTo('actionWithArgs', ['foo', 'bar']);
        $this->assertEquals('actionWithArgs', $this->object->getCurrentAction());
        $this->assertEquals(['foo', 'bar'], $this->object->getCurrentArguments());

        $this->assertEquals([], $this->object->getActionArguments('actionNoArgs'));
        $this->assertEquals(['foo', 'bar'], $this->object->getActionArguments('actionWithArgs'));
        $this->assertEquals([], $this->object->getActionArguments('noAction'));
    }

    public function testRenderErrorWithNoReporting() {
        $old = error_reporting(0);

        $this->assertEquals('404: Not Found', $this->object->renderError(new NotFoundException('Not Found')));
        $this->assertEquals(404, $this->object->getResponse()->getStatusCode());

        error_reporting($old);
    }

    public function testRenderErrorWithReporting() {
        $old = error_reporting(E_ALL);

        $this->assertEquals('Message', $this->object->renderError(new \Exception('Message')));
        $this->assertEquals(500, $this->object->getResponse()->getStatusCode());

        error_reporting($old);
    }

    public function testRenderView() {
        $this->assertEquals('stub:index', $this->object->renderView());

        $this->object->dispatchTo('actionNoArgs', []);
        $this->assertEquals('stub:action-no-args', $this->object->renderView());
    }

    /**
     * @expectedException \Titon\View\Exception\MissingTemplateException
     */
    public function testRenderViewMissingView() {
        $this->object->dispatchTo('actionWithArgs', ['foo', 'bar']);
        $this->assertEquals('stub:action-with-args', $this->object->renderView());
    }

    public function testGetSetView() {
        $stub = new ControllerStub();
        $view = new EngineView($this->vfs->path('/views/'));

        $this->assertEquals(null, $stub->getView());

        $stub->setView($view);
        $this->assertEquals($view, $stub->getView());
    }

}
