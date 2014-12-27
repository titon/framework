<?php
namespace Titon\Cache\Storage;

class FileSystemStorageTest extends AbstractStorageTest {

    protected function setUp() {
        $this->setupVFS();
        $this->vfs->createDirectory('/cache/');

        $this->object = new FileSystemStorage($this->vfs->path('/cache/'));

        parent::setUp();
    }

    /**
     * @expectedException \Titon\Io\Exception\InvalidPathException
     */
    public function testExceptionThrownMissingDir() {
        new FileSystemStorage('');
    }

    public function testFlush() {
        $this->assertFileExists($this->vfs->path('/cache/foo.cache'));

        $this->object->flush();

        $this->assertFileNotExists($this->vfs->path('/cache/foo.cache'));
    }

    public function testRemove() {
        $this->assertTrue($this->object->has('foo'));
        $this->assertFileExists($this->vfs->path('/cache/foo.cache'));

        $this->object->remove('foo');

        $this->assertFalse($this->object->has('foo'));
        $this->assertFileNotExists($this->vfs->path('/cache/foo.cache'));
    }

}