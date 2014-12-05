<?php
namespace Titon\Cache\Storage;

use VirtualFileSystem\FileSystem;

/**
 * @property \VirtualFileSystem\FileSystem $vfs
 */
class FileSystemStorageTest extends AbstractStorageTest {

    protected function setUp() {
        $this->markTestSkipped('Requires the io package');

        $this->vfs = new FileSystem();
        $this->vfs->createDirectory('/cache/');

        $this->object = new FileSystemStorage($this->vfs->path('/cache/'));

        parent::setUp();
    }

    public function testExceptionThrownMissingDir() {
        new FileSystemStorage('');
    }

    public function testFlush() {
        $this->assertFileExists(TEMP_DIR . '/User.getById.1337.cache');
        $this->object->flush();
        $this->assertFileNotExists(TEMP_DIR . '/User.getById.1337.cache');
    }

    public function testRemove() {
        $this->assertTrue($this->object->has('User::getById-1337'));
        $this->assertFileExists(TEMP_DIR . '/User.getById.1337.cache');

        $this->object->remove('User::getById-1337');

        $this->assertFalse($this->object->has('User::getById-1337'));
        $this->assertFileNotExists(TEMP_DIR . '/User.getById.1337.cache');
    }

}