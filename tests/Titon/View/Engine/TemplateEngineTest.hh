<?hh
namespace Titon\View\Engine;

use Titon\View\EngineView;
use Titon\Test\TestCase;
use VirtualFileSystem\FileSystem;

/**
 * @property \Titon\View\EngineView $object
 * @property \Titon\View\Engine\TemplateEngine $engine
 */
class TemplateEngineTest extends TestCase {

    protected function setUp() {
        parent::setUp();

        $this->vfs = new FileSystem();
        $this->vfs->createStructure([
            '/views/' => [
                'private/' => [
                    'partials/' => [
                        'nested/' => [
                            'include.tpl' => 'nested/include.tpl'
                        ],
                        'variables.tpl' => '<?php echo $name; ?> - <?php echo $type; ?> - <?php echo $filename; ?>'
                    ]
                ],
                'public/' => [
                    'index/' => [
                        'add.tpl' => 'add.tpl',
                        'test-include.tpl' => 'test-include.tpl <?php echo $this->open(\'nested/include\'); ?>'
                    ]
                ]
            ]
        ]);

        $this->engine = new TemplateEngine();

        $this->object = new EngineView([$this->vfs->path('/views')]);
        $this->object->setEngine($this->engine);
    }

    public function testOpen() {
        $this->assertEquals('nested/include.tpl', $this->object->getEngine()->open('nested/include'));
        $this->assertEquals('nested/include.tpl', $this->object->getEngine()->open('nested/include.tpl'));
        $this->assertEquals('Titon - partial - variables.tpl', $this->object->getEngine()->open('variables', Map {
            'name' => 'Titon',
            'type' => 'partial',
            'filename' => 'variables.tpl'
        }));
    }

    /**
     * @expectedException \Titon\View\Exception\MissingTemplateException
     */
    public function testOpenMissingFile() {
        $this->object->getEngine()->open('foobar');
    }

    public function testRender() {
        $this->assertEquals('add.tpl', $this->object->renderTemplate($this->object->locateTemplate(['index', 'add'])));
        $this->assertEquals('test-include.tpl nested/include.tpl', $this->object->renderTemplate($this->object->locateTemplate(['index', 'test-include'])));
    }

    public function testData() {
        $this->assertEquals('add.tpl', $this->engine->render($this->object->locateTemplate(['index', 'add']), Map {
            'foo' => 'bar'
        }));

        $this->assertEquals('bar', $this->engine->data('foo'));
        $this->assertEquals(null, $this->engine->data('key'));
        $this->assertEquals('default', $this->engine->data('key', 'default'));
    }

}