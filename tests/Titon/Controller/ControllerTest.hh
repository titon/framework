<?hh
namespace Titon\Controller;

use Titon\Http\Exception\NotFoundException;
use Titon\Http\Server\Request;
use Titon\Http\Server\Response;
use Titon\Test\Stub\Controller\ControllerStub;
use Titon\Test\TestCase;
use Titon\View\Engine\TemplateEngine;
use Titon\View\EngineView;
use Titon\View\Locator\TemplateLocator;

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
                        'error.tpl' => '<?= $message; ?>',
                        'http.tpl' => '<?= $code . \': \' . $message; ?>'
                    ],
                    'layouts/' => [
                        'default.tpl' => '<?= $this->getContent(); ?>'
                    ]
                ],
                'public/' => [
                    'stub/' => [
                        'returns-nothing.tpl' => 'This view was automatically rendered.',
                        'action-no-args.tpl' => 'stub:action-no-args',
                        'index.tpl' => 'stub:index'
                    ]
                ]
            ]
        ]);

        $view = new EngineView(new TemplateLocator($this->vfs()->path('/views/')));
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

    public function testDispatchToCatchesExceptions(): void {
        $this->assertEquals('Your action noAction does not exist. Supply your own `missingAction()` method to customize this error or view.', (string) $this->object->dispatchTo('noAction', [])->getBody()); // Missing action
    }

    public function testDispatchToSetsStreams(): void {
        $oldResponse = $this->object->getResponse();

        $this->assertEquals('', (string) $oldResponse->getBody());

        $newResponse = $this->object->dispatchTo('returnsStream', []);

        $this->assertEquals('returnsStream', (string) $newResponse->getBody());
        $this->assertSame($oldResponse, $newResponse);
    }

    public function testDispatchToSetsNewResponses(): void {
        $oldResponse = $this->object->getResponse();

        $this->assertEquals('', (string) $oldResponse->getBody());

        $newResponse = $this->object->dispatchTo('returnsResponse', []);

        $this->assertEquals('["returnsResponse"]', (string) $newResponse->getBody());
        $this->assertNotSame($oldResponse, $newResponse);
        $this->assertSame($newResponse, $this->object->getResponse());
    }

    public function testDispatchToAutoRendersView(): void {
        $this->assertEquals('This view was automatically rendered.', (string) $this->object->dispatchTo('returnsNothing', [])->getBody());
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

        $this->assertEquals('404: Not Found', $this->object->renderError(new NotFoundException('Not Found')));
        $this->assertEquals(404, $this->object->getResponse()->getStatusCode());

        error_reporting($old);
    }

    public function testRenderErrorWithReporting(): void {
        $old = error_reporting(E_ALL);

        $this->assertEquals('Message', $this->object->renderError(new \Exception('Message')));
        $this->assertEquals(500, $this->object->getResponse()->getStatusCode());

        error_reporting($old);
    }

    public function testRenderView(): void {
        $this->assertEquals('stub:index', $this->object->renderView());

        $this->object->dispatchTo('actionNoArgs', []);
        $this->assertEquals('stub:action-no-args', $this->object->renderView());
    }

    /**
     * @expectedException \Titon\View\Exception\MissingTemplateException
     */
    public function testRenderViewMissingView(): void {
        $this->object->dispatchTo('actionWithArgs', ['foo', 'bar']);
        $this->assertEquals('stub:action-with-args', $this->object->renderView());
    }

    public function testGetSetView(): void {
        $view = new EngineView(new TemplateLocator($this->vfs()->path('/other-views/')));

        $this->assertNotEquals($view, $this->object->getView());

        $this->object->setView($view);

        $this->assertEquals($view, $this->object->getView());
    }

}
