<?php
namespace Titon\Cache\Storage;

class RedisStorageTest extends AbstractStorageTest {

    protected function setUp() {
        if (!extension_loaded('redis')) {
            $this->markTestSkipped('Redis is not installed or configured properly');
        }

        $this->object = new RedisStorage([
            'server' => '127.0.0.1:6379'
        ]);

        // Check that memcache connected
        $this->assertEquals('+PONG', $this->object->connection->ping());

        // Redis requires serializing data
        $this->assertEquals(true, $this->object->set('User::getById-1337', serialize(['username' => 'Titon']), '+5 minutes'));
        $this->assertEquals(true, $this->object->set('Topic::getAll', serialize([['id' => 1], ['id' => 2]]), '-1 day')); // expired
        $this->assertEquals(true, $this->object->set('Comment::count', 1, '+5 minutes'));
    }

    public function testDecrementMissingKey() {
        // Redis creates the key and decrements it
        $this->assertEquals(-1, $this->object->decrement('Fake::key'));
    }

    public function testGet() {
        $this->assertEquals(['username' => 'Titon'], unserialize($this->object->get('User::getById-1337')));
        $this->assertEquals(1, $this->object->get('Comment::count'));

        $this->assertEquals(null, $this->object->get('Topic::getAll')); // Expired
        $this->assertEquals(null, $this->object->get('Post::getById-666')); // Key doesn't exist
    }

    public function testSet() {
        $this->assertEquals(['username' => 'Titon'], unserialize($this->object->get('User::getById-1337')));
        $this->object->set('User::getById-1337', serialize(['username' => 'Titon Framework']));
        $this->assertEquals(['username' => 'Titon Framework'], unserialize($this->object->get('User::getById-1337')));

        $this->assertEquals(null, $this->object->get('Post::getById-666'));
        $this->object->set('Post::getById-666', serialize(['username' => 'Miles']));
        $this->assertEquals(['username' => 'Miles'], unserialize($this->object->get('Post::getById-666')));
    }

    public function testIncrementMissingKey() {
        // Redis creates the key and increments it
        $this->assertEquals(1, $this->object->increment('Fake::key'));
    }

}