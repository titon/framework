<?hh
namespace Titon\Io;

use Titon\Test\TestCase;
use VirtualFileSystem\FileSystem;

class NodeTest extends TestCase {

    protected function setUp(): void {
        parent::setUp();

        $this->vfs = new FileSystem();
        $this->vfs()->createDirectory('/folder');
        $this->vfs()->createFile('/file');
    }

    public function testDestroy(): void {
        $this->assertFalse(Node::destroy($this->vfs()->path('missing')));

        $this->assertFileExists($this->vfs()->path('folder'));
        $this->assertTrue(Node::destroy($this->vfs()->path('folder')));
        $this->assertFileNotExists($this->vfs()->path('folder'));

        $this->assertFileExists($this->vfs()->path('file'));
        $this->assertTrue(Node::destroy($this->vfs()->path('file')));
        $this->assertFileNotExists($this->vfs()->path('file'));
    }

    public function testLoad(): void {
        $this->assertInstanceOf('Titon\Io\Folder', Node::load($this->vfs()->path('folder')));
        $this->assertInstanceOf('Titon\Io\File', Node::load($this->vfs()->path('file')));
    }

    /**
     * @expectedException \Titon\Io\Exception\MissingFileException
     */
    public function testLoadMissingTarget(): void {
        Node::load($this->vfs()->path('missing'));
    }

}
