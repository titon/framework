<?php
namespace Titon\Event;

use Titon\Test\TestCase;

/**
 * @property \Titon\Event\Event $object
 */
class EventTest extends TestCase {

    protected $time;

    protected function setUp() {
        parent::setUp();

        $this->time = time();
        $this->object = new Event('event.test.key', [
            ['callback' => 'ClassName::method3', 'priority' => 10],
            ['callback' => 'ClassName::method1', 'priority' => 100],
            ['callback' => 'ClassName::method2', 'priority' => 110]
        ]);
    }

    public function testGetCallstack() {
        $this->assertEquals([
            ['callback' => 'ClassName::method3', 'priority' => 10],
            ['callback' => 'ClassName::method1', 'priority' => 100],
            ['callback' => 'ClassName::method2', 'priority' => 110]
        ], $this->object->getCallStack());
    }

    public function testGetIndexAndNext() {
        $this->assertEquals(0, $this->object->getIndex());

        $this->object->next();
        $this->assertEquals(1, $this->object->getIndex());

        $this->object->next();
        $this->object->next();
        $this->object->next();
        $this->object->next();
        $this->assertEquals(2, $this->object->getIndex());
    }

    public function testIsStoppedAndStop() {
        $this->assertFalse($this->object->isStopped());

        $this->object->stop();
        $this->assertTrue($this->object->isStopped());
    }

    public function testGetKey() {
        $this->assertEquals('event.test.key', $this->object->getKey());
    }

    public function testGetTime() {
        $this->assertEquals($this->time, $this->object->getTime());
    }

}