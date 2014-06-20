<?php
namespace Titon\Type;

use Titon\Test\TestCase;

/**
 * @property \Titon\Type\Collection $object
 */
class CollectionTest extends TestCase {

    protected function setUp() {
        parent::setUp();

        $this->object = new Collection([
            new String('Foo'),
            new String('Bar'),
            new String('Baz'),
            new String('Abc'),
            new String('Xyz')
        ]);
    }

    public function testTrigger() {
        $this->object->trigger('toUpperCase');

        $this->assertEquals([
            'FOO',
            'BAR',
            'BAZ',
            'ABC',
            'XYZ'
        ], array_map('strval', $this->object->value()));
    }

}