<?hh
namespace Titon\Io;

use Titon\Test\TestCase;
use VirtualFileSystem\FileSystem;

class NodeTest extends TestCase {

    protected function setUp() {
        parent::setUp();

        $this->vfs = new FileSystem();
        $this->vfs->createDirectory('/folder');
        $this->vfs->createFile('/file');
    }

    public function testDestroy() {
        $this->assertFalse(Node::destroy($this->vfs->path('missing')));

        $this->assertFileExists($this->vfs->path('folder'));
        $this->assertTrue(Node::destroy($this->vfs->path('folder')));
        $this->assertFileNotExists($this->vfs->path('folder'));

        $this->assertFileExists($this->vfs->path('file'));
        $this->assertTrue(Node::destroy($this->vfs->path('file')));
        $this->assertFileNotExists($this->vfs->path('file'));
    }

    public function testLoad() {
        $this->assertInstanceOf('Titon\Io\Folder', Node::load($this->vfs->path('folder')));
        $this->assertInstanceOf('Titon\Io\File', Node::load($this->vfs->path('file')));
    }

    /**
     * @expectedException \Titon\Common\Exception\MissingFileException
     */
    public function testLoadMissingTarget() {
        Node::load($this->vfs->path('missing'));
    }

}