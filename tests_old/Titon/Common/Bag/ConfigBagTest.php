<?php
namespace Titon\Common\Bag;

use Titon\Test\TestCase;

/**
 * @property \Titon\Common\Bag\ConfigBag $object
 */
class ConfigBagTest extends TestCase {

    protected $defaults = [
        'boolean' => true,
        'integer' => 12345,
        'string' => 'foobar',
        'float' => 50.25,
        'array' => [
            'key' => 'value'
        ]
    ];

    protected function setUp() {
        parent::setUp();

        $this->object = new ConfigBag([], $this->defaults);
    }

    public function testGetDefaults() {
        $this->object->set('boolean', false);

        $this->assertEquals($this->defaults, $this->object->getDefaults());
    }

}