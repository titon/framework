<?php
namespace Titon\View\View\Engine;

use Titon\View\View;
use Titon\View\View\TemplateView;
use Titon\Test\TestCase;

/**
 * @property \Titon\View\View\TemplateView $object
 * @property \Titon\View\View\Engine\TemplateEngine $engine
 */
class TemplateEngineTest extends TestCase {

    protected function setUp() {
        parent::setUp();

        $this->engine = new TemplateEngine();

        $this->object = new TemplateView([
            TEMP_DIR,
            TEMP_DIR . '/fallback'
        ]);
        $this->object->setEngine($this->engine);
    }

    public function testOpen() {
        $this->assertEquals('nested/include.tpl', $this->object->getEngine()->open('nested/include'));
        $this->assertEquals('nested/include.tpl', $this->object->getEngine()->open('nested/include.tpl'));
        $this->assertEquals('Titon - partial - variables.tpl', $this->object->getEngine()->open('variables', [
            'name' => 'Titon',
            'type' => 'partial',
            'filename' => 'variables.tpl'
        ]));
    }

    public function testOpenMissingFile() {
        $this->object->getEngine()->open('foobar');
    }

    public function testRender() {
        $this->assertEquals('add.tpl', $this->object->renderTemplate($this->object->locateTemplate(['index', 'add'])));
        $this->assertEquals('test-include.tpl nested/include.tpl', $this->object->renderTemplate($this->object->locateTemplate(['index', 'test-include'])));
    }

    public function testRenderMissingFile() {
        $this->object->renderTemplate($this->object->locateTemplate(['index', 'add']));
    }

    public function testData() {
        $this->assertEquals('add.tpl', $this->engine->render($this->object->locateTemplate(['index', 'add']), [
            'foo' => 'bar'
        ]));

        $this->assertEquals('bar', $this->engine->data('foo'));
        $this->assertEquals(null, $this->engine->data('key'));
        $this->assertEquals('default', $this->engine->data('key', 'default'));
    }

}