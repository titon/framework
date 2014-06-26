<?php
namespace Titon\View\View;

use Titon\Test\TestCase;
use Titon\View\View\Engine\AbstractEngine;

/**
 * @property \Titon\View\View\Engine $object
 */
class EngineTest extends TestCase {

    protected function setUp() {
        parent::setUp();

        $this->object = new EngineStub();
    }

    public function testLayout() {
        $this->assertEquals('default', $this->object->getLayout());

        $this->object->useLayout('alternate');
        $this->assertEquals('alternate', $this->object->getLayout());
    }

    public function testWrapper() {
        $this->assertEquals([], $this->object->getWrapper());

        $this->object->wrapWith('alternate');
        $this->assertEquals(['alternate'], $this->object->getWrapper());

        $this->object->wrapWith(['alternate', 'double']);
        $this->assertEquals(['alternate', 'double'], $this->object->getWrapper());
    }

    public function testContent() {
        $this->assertEquals('', $this->object->getContent());

        $this->object->setContent('content');
        $this->assertEquals('content', $this->object->getContent());
    }

}

class EngineStub extends AbstractEngine {

    public function open($partial, array $variables = array()) {}
    public function render($path, array $variables = array()) {}
    public function getExtension() {}

}