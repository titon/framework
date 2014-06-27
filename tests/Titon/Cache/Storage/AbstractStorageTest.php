<?php
namespace Titon\Cache\Storage;

use Titon\Test\TestCase;

/**
 * @property \Titon\Cache\Storage $object
 */
abstract class AbstractStorageTest extends TestCase {

    protected function setUp() {
        parent::setUp();

        // Assert true so we know cache is being written
        $this->assertEquals(true, $this->object->set('User::getById-1337', ['username' => 'Titon'], '+5 minutes'));
        $this->assertEquals(true, $this->object->set('Topic::getAll', [['id' => 1], ['id' => 2]], '-1 day')); // expired
        $this->assertEquals(true, $this->object->set('Comment::count', 1, '+5 minutes'));
    }

    protected function tearDown() {
        if ($this->object) {
            $this->object->flush();
            unset($this->object);
        }
    }

    public function testDecrement() {
        $this->assertEquals(1, $this->object->get('Comment::count'));
        $this->assertEquals(0, $this->object->decrement('Comment::count', 1));
        $this->assertEquals(-5, $this->object->decrement('Comment::count', 5));
    }

    public function testDecrementMissingKey() {
        $this->assertSame(false, $this->object->decrement('Fake::key'));
    }

    public function testFlush() {
        $this->assertTrue($this->object->has('User::getById-1337'));
        $this->object->flush();
        $this->assertFalse($this->object->has('User::getById-1337'));
    }

    public function testGet() {
        $this->assertEquals(['username' => 'Titon'], $this->object->get('User::getById-1337'));
        $this->assertEquals(1, $this->object->get('Comment::count'));
    }

    public function testGetMissingKey() {
        $this->assertEquals(null, $this->object->get('Topic::getAll')); // Expired
        $this->assertEquals(null, $this->object->get('Post::getById-666')); // Key doesn't exist
    }

    public function testHas() {
        $this->assertTrue($this->object->has('User::getById-1337'));
        $this->assertFalse($this->object->has('Post::getById-666'));
    }

    public function testIncrement() {
        $this->assertEquals(1, $this->object->get('Comment::count'));
        $this->assertEquals(2, $this->object->increment('Comment::count', 1));
        $this->assertEquals(7, $this->object->increment('Comment::count', 5));
    }

    public function testIncrementMissingKey() {
        $this->assertSame(false, $this->object->increment('Fake::key'));
    }

    public function testRemove() {
        $this->assertTrue($this->object->has('User::getById-1337'));
        $this->object->remove('User::getById-1337');
        $this->assertFalse($this->object->has('User::getById-1337'));
    }

    public function testSet() {
        $this->assertEquals(['username' => 'Titon'], $this->object->get('User::getById-1337'));
        $this->object->set('User::getById-1337', ['username' => 'Titon Framework']);
        $this->assertEquals(['username' => 'Titon Framework'], $this->object->get('User::getById-1337'));

        $this->assertEquals(null, $this->object->get('Post::getById-666'));
        $this->object->set('Post::getById-666', ['username' => 'Miles']);
        $this->assertEquals(['username' => 'Miles'], $this->object->get('Post::getById-666'));
    }

    public function testStats() {
        $this->assertTrue(is_array($this->object->stats()));
    }

    public function testStore() {
        $this->assertEquals('foo', $this->object->store('storeTest', function() {
            return 'foo';
        }));

        $this->assertEquals('foo', $this->object->store('storeTest', function() {
            return 'bar';
        }));

        $this->assertEquals('foo', $this->object->store('storeTest', function() {
            return 'baz';
        }));
    }

}