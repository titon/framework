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

    protected function setUp(): void {
        parent::setUp();

        $this->vfs()->createStructure([
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

        $view = new EngineView($this->vfs()->path('/views/'));
        $view->setEngine(new TemplateEngine());

        $this->object = new ControllerStub(Request::createFromGlobals(), new Response());
        $this->object->setView($view);
    }

    public function testDispatchTo(): void {
        $this->assertEquals('actionNoArgs', (string) $this->object->dispatchTo('action-no-args', [])->getBody());
        $this->assertEquals('actionNoArgs', (string) $this->object->dispatchTo('actionNoArgs', [])->getBody());
        $this->assertEquals('actionNoArgs', (string) $this->object->dispatchTo('actionNoArgs', ['foo', 'bar'])->getBody());
        $this->assertEquals(125, (string) $this->object->dispatchTo('actionWithArgs', [125])->getBody());
        $this->assertEquals(555, (string) $this->object->dispatchTo('actionWithArgs', [505, 50])->getBody());
        $this->assertEquals(335, (string) $this->object->dispatchTo('actionWithArgs', [335])->getBody());
        $this->assertEquals('', (string) $this->object->dispatchTo('actionWithArgs', ['foo', 'bar'])->getBody());
    }

    /**
     * @expectedException \Titon\Controller\Exception\InvalidActionException
     */
    public function testDispatchToMissingAction(): void {
        $this->object->dispatchTo('noAction', []);
    }

    public function testForwardTo(): void {
        $this->object->forwardTo('actionNoArgs', []);
        $this->assertEquals('actionNoArgs', $this->object->getCurrentAction());

        $this->object->forwardTo('actionWithArgs', ['foo']);
        $this->assertEquals('actionWithArgs', $this->object->getCurrentAction());
    }

    public function testGetActionAndArguments(): void {
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

    public function testRenderErrorWithNoReporting(): void {
        $old = error_reporting(0);

        $this->assertEquals('404: Not Found', $this->object->renderError(new NotFoundException('Not Found'))->getBody());
        $this->assertEquals(404, $this->object->getResponse()->getStatusCode());

        error_reporting($old);
    }

    public function testRenderErrorWithReporting(): void {
        $old = error_reporting(E_ALL);

        $this->assertEquals('Message', $this->object->renderError(new \Exception('Message'))->getBody());
        $this->assertEquals(500, $this->object->getResponse()->getStatusCode());

        error_reporting($old);
    }

    public function testRenderView(): void {
        $this->assertEquals('stub:index', $this->object->renderView()->getBody());

        $this->object->dispatchTo('actionNoArgs', []);
        $this->assertEquals('stub:action-no-args', $this->object->renderView()->getBody());
    }

    /**
     * @expectedException \Titon\View\Exception\MissingTemplateException
     */
    public function testRenderViewMissingView(): void {
        $this->object->dispatchTo('actionWithArgs', ['foo', 'bar']);
        $this->assertEquals('stub:action-with-args', $this->object->renderView());
    }

    public function testGetSetView(): void {
        $view = new EngineView($this->vfs()->path('/other-views/'));

        $this->assertNotEquals($view, $this->object->getView());

        $this->object->setView($view);

        $this->assertEquals($view, $this->object->getView());
    }

}
