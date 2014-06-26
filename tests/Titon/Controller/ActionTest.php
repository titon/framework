<?php
namespace Titon\Controller;

use Titon\Controller\Action\AbstractAction;
use Titon\Controller\Controller\ErrorController;
use Titon\Test\TestCase;

class ActionTest extends TestCase {

    public function testRun() {
        $controller = new ErrorController(['foo' => 'bar']);

        $this->assertEquals('bar', $controller->getConfig('foo'));
        $this->assertArrayNotHasKey('test', $controller->allConfig());

        $action = new ActionStub();
        $action->setController($controller)->get();

        $this->assertEquals($controller, $action->getController());

        $this->assertNotEquals('bar', $controller->getConfig('foo'));
        $this->assertEquals('baz', $controller->getConfig('foo'));
        $this->assertArrayHasKey('test', $controller->allConfig());
    }

}

class ActionStub extends AbstractAction {

    public function get() {
        $this->getController()->addConfig([
            'foo' => 'baz',
            'test' => 'value'
        ]);
    }

    public function post() {}

    public function delete() {}

    public function put() {}

}