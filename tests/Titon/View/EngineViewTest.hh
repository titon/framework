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
            $this->vfs()->path('/views/fallback/')
        ]));
    }

    public function testRender(): void {
        $this->assertEquals('<layout>edit.tpl</layout>', $this->object->render('index/edit'));

        $this->object->getEngine()->useLayout('fallback');
        $this->assertEquals('<fallbackLayout>add.tpl</fallbackLayout>', $this->object->render('index/add'));

        $this->object->getEngine()->wrapWith('wrapper');
        $this->assertEquals('<fallbackLayout><wrapper>index.tpl</wrapper></fallbackLayout>', $this->object->render('index/index'));

        $this->object->getEngine()->wrapWith('wrapper', 'fallback');
        $this->assertEquals('<fallbackLayout><fallbackWrapper><wrapper>view.tpl</wrapper></fallbackWrapper></fallbackLayout>', $this->object->render('index/view'));

        $this->object->getEngine()->wrapWith()->useLayout('');
        $this->assertEquals('view.xml.tpl', $this->object->render('index/view.xml'));
    }

    public function testRenderPrivate(): void {
        $this->assertEquals('<layout>public/root.tpl</layout>', $this->object->render('root'));
        $this->assertEquals('<layout>private/root.tpl</layout>', $this->object->render('root', true));
    }

    public function testRenderTemplate(): void {
        $this->assertEquals('add.tpl', $this->object->renderTemplate($this->object->getLocator()->locate('index/add')));
        $this->assertEquals('test-include.tpl nested/include.tpl', $this->object->renderTemplate($this->object->getLocator()->locate('index/test-include')));

        // variables
        $this->assertEquals('Titon - partial - variables.tpl', $this->object->renderTemplate($this->object->getLocator()->locate('variables', Template::PARTIAL), Map {
            'name' => 'Titon',
            'type' => 'partial',
            'filename' => 'variables.tpl'
        }));
    }

    public function testViewCaching(): void {
        $storage = new MemoryStorage();

        $this->object->setStorage($storage);

        $path = $this->object->getLocator()->locate('index/test-include');
        $key = md5($path);

        $this->assertFalse($storage->has($key));

        $this->assertEquals('test-include.tpl nested/include.tpl', $this->object->renderTemplate($path));

        $this->assertTrue($storage->has($key));

        // Validate it returns the cached value
        $this->assertEquals('test-include.tpl nested/include.tpl', $this->object->renderTemplate($path));

        $storage->flush();

        $this->assertFalse($storage->has($key));
    }

}
