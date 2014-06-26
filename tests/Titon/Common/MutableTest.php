<?php
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Common;

use Titon\Test\TestCase;
use \ArrayAccess;
use \IteratorAggregate;
use \Countable;

/**
 * @property \Titon\Common\MutableStub $object
 */
class MutableTest extends TestCase {

    protected function setUp() {
        parent::setUp();

        $this->object = new MutableStub();
    }

    public function testAddAll() {
        $this->object->set('foo', 'bar');

        $this->assertEquals(['foo' => 'bar'], $this->object->all());

        $this->object->add([
            'foo' => 'baz',
            'depth.key' => 'value'
        ]);

        $this->assertEquals([
            'foo' => 'baz',
            'depth' => ['key' => 'value']
        ], $this->object->all());
    }

    public function testFlush() {
        $this->object->set('foo', 'bar');

        $this->assertEquals(['foo' => 'bar'], $this->object->all());

        $this->object->flush();

        $this->assertEquals([], $this->object->all());
    }

    public function testGet() {
        $this->object->set('foo', 'bar');

        $this->assertEquals('bar', $this->object->get('foo'));
        $this->assertEquals('bar', $this->object->foo);
        $this->assertEquals('bar', $this->object['foo']);

        $this->assertEquals('default', $this->object->get('missing', 'default'));
    }

    public function testHasRemove() {
        $this->object->set('foo', 'bar');
        $this->object->set('key', 'value');

        $this->assertTrue($this->object->has('foo'));
        $this->assertTrue(isset($this->object['key']));

        $this->object->remove('foo');
        unset($this->object['key']);

        $this->assertFalse($this->object->has('foo'));
        $this->assertFalse(isset($this->object['key']));
    }

    public function testSet() {
        $this->object->set('a', 1);
        $this->object->b = 2;
        $this->object['c'] = 3;

        $this->assertEquals(['a' => 1, 'b' => 2, 'c' => 3], $this->object->toArray());
        $this->assertEquals(['a', 'b', 'c'], $this->object->keys());
    }

}

class MutableStub implements ArrayAccess, IteratorAggregate, Countable {
    use Mutable;
}