<?hh
namespace Titon\View;

use Titon\Cache\Storage\MemoryStorage;
use Titon\Test\TestCase;
use Titon\View\Locator\TemplateLocator;

/**
 * @property \Titon\View\EngineView $object
 */
class EngineViewTest extends TestCase {

    protected function setUp(): void {
        parent::setUp();

        $this->vfs()->createStructure(ViewTest::generateViewStructure());

        $this->object = new EngineView(new TemplateLocator([
            $this->vfs()->path('/views/'),
            $this->vfs()->path('/views-fallback/')
        ]));
    }

    public function testRender(): void {
        $this->assertEquals('<layout>public.index.edit</layout>', $this->object->render('index/edit'));

        $this->object->getEngine()->useLayout('fallback');
        $this->assertEquals('<fallbackLayout>public.index.add</fallbackLayout>', $this->object->render('index/add'));

        $this->object->getEngine()->wrapWith('wrapper');
        $this->assertEquals('<fallbackLayout><wrapper>public.index.index</wrapper></fallbackLayout>', $this->object->render('index/index'));

        $this->object->getEngine()->wrapWith('wrapper', 'fallback');
        $this->assertEquals('<fallbackLayout><fallbackWrapper><wrapper>public.index.view</wrapper></fallbackWrapper></fallbackLayout>', $this->object->render('index/view'));

        $this->object->getEngine()->wrapWith()->useLayout('');
        $this->assertEquals('public.index.view.xml', $this->object->render('index/view.xml'));
    }

    public function testRenderPrivate(): void {
        $this->assertEquals('<layout>public.root</layout>', $this->object->render('root'));
        $this->assertEquals('<layout>private.root</layout>', $this->object->render('root', true));
    }

    public function testRenderTemplate(): void {
        $this->assertEquals('public.index.add', $this->object->renderTemplate($this->object->getLocator()->locate('index/add')));
        $this->assertEquals('public.index.include - private.partials.nested.include', $this->object->renderTemplate($this->object->getLocator()->locate('index/include')));

        $this->assertEquals('Titon - partial - variables.tpl', $this->object->renderTemplate($this->object->getLocator()->locate('variables', Template::PARTIAL), Map {
            'name' => 'Titon',
            'type' => 'partial',
            'filename' => 'variables.tpl'
        }));
    }

    public function testViewCaching(): void {
        $storage = new MemoryStorage();

        $this->object->setStorage($storage);

        $path = $this->object->getLocator()->locate('index/include');
        $key = md5($path);

        $this->assertFalse($storage->has($key));

        $this->assertEquals('public.index.include - private.partials.nested.include', $this->object->renderTemplate($path));

        $this->assertTrue($storage->has($key));

        // Validate it returns the cached value
        $this->assertEquals('public.index.include - private.partials.nested.include', $this->object->renderTemplate($path));

        $storage->flush();

        $this->assertFalse($storage->has($key));
    }

}
