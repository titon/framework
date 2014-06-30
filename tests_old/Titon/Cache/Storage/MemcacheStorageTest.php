<?php
namespace Titon\Cache\Storage;

class MemcacheStorageTest extends AbstractStorageTest {

    protected function setUp() {
        if (!extension_loaded('memcached')) {
            $this->markTestSkipped('Memcache is not installed or configured properly');
        }

        $this->object = new MemcacheStorage([
            'server' => '127.0.0.1:11211'
        ]);

        // Check that memcache connected
        $stats = $this->object->connection->getStats();

        if (empty($stats['127.0.0.1:11211'])) {
            throw new \Exception('Could not connect to Memcache');
        }

        parent::setUp();
    }

    /**
     * Memcache wont decrement below 0.
     */
    public function testDecrement() {
        $this->assertEquals(1, $this->object->get('Comment::count'));

        $this->object->decrement('Comment::count', 1);
        $this->assertEquals(0, $this->object->get('Comment::count'));

        $this->object->decrement('Comment::count', 5);
        $this->assertEquals(0, $this->object->get('Comment::count')); // changed from -5
    }

}