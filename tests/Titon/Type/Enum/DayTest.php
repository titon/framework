<?php
namespace Titon\Type\Enum;

use Titon\Test\TestCase;

/**
 * @property \Titon\Type\Enum\Day $object
 */
class DayTest extends TestCase {

    protected function setUp() {
        parent::setUp();

        $this->object = new Day(Day::SATURDAY);
    }

    public function testVariables() {
        $this->assertEquals(6, $this->object->order);
        $this->assertEquals('Saturday', $this->object->name);
        $this->assertEquals('Sat', $this->object->shortName);
        $this->assertEquals('saturday', $this->object->slug);
    }

    public function testIs() {
        $this->assertTrue($this->object->is(Day::SATURDAY));
        $this->assertFalse($this->object->is(Day::WEDNESDAY));
    }

}