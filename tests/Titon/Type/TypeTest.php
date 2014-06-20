<?php
namespace Titon\Type;

use Titon\Test\TestCase;

/**
 * @property \Titon\Type\Type $object
 */
class TypeTest extends TestCase {

    protected function setUp() {
        parent::setUp();

        $this->object = new Type('Titon Framework');
    }

    public function testSerialize() {
        $this->assertEquals('s:15:"Titon Framework";', $this->object->serialize());
    }

    public function testToString() {
        $this->assertEquals('Titon Framework', $this->object->toString());
        $this->assertEquals('Titon Framework', (string) $this->object);
    }

    public function testUnserialize() {
        $this->object->unserialize('s:5:"Titon";');

        $this->assertEquals('Titon', $this->object->value());
    }

    public function testValue() {
        $this->assertEquals('Titon Framework', $this->object->value());

        $this->object->write('Titon PHP');
        $this->assertEquals('Titon PHP', $this->object->value());
    }

}