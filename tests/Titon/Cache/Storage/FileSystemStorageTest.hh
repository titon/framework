<?hh // strict
namespace Titon\Cache\Storage;

class FileSystemStorageTest extends AbstractStorageTest {

    protected function setUp(): void {
        $this->setupVFS();
        $this->vfs->createDirectory('/cache/');

        $this->object = new FileSystemStorage($this->vfs->path('/cache/'), 'fs-');

        parent::setUp();
    }

    /**
     * @expectedException \Titon\Io\Exception\InvalidPathException
     */
    public function testExceptionThrownMissingDir(): void {
        new FileSystemStorage('');
    }

    public function testFlush(): void {
        $this->assertFileExists($this->vfs->path('/cache/fs-foo.cache'));

        $this->object->flush();

        $this->assertFileNotExists($this->vfs->path('/cache/fs-foo.cache'));
    }

    public function testRemove(): void {
        $this->assertTrue($this->object->has('foo'));
        $this->assertFileExists($this->vfs->path('/cache/fs-foo.cache'));

        $this->object->remove('foo');

        $this->assertFalse($this->object->has('foo'));
        $this->assertFileNotExists($this->vfs->path('/cache/fs-foo.cache'));
    }

}
