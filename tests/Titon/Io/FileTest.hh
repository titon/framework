<?hh
namespace Titon\Io;

use Titon\Test\TestCase;
use VirtualFileSystem\FileSystem;

/**
 * @property \Titon\Io\File $object
 * @property \Titon\Io\File $temp
 */
class FileTest extends TestCase {

    protected function setUp(): void {
        parent::setUp();

        $this->object = new File($this->vfs()->path('file/base.html'), true, 0777);
        $this->temp = new File($this->vfs()->path('file/temp.html'), false);
    }

    /**
     * @expectedException \Titon\Io\Exception\InvalidPathException
     */
    public function testConstructThrowsNotFileException(): void {
        new File(TEMP_DIR);
    }

    public function testAppendPrepend(): void {
        $this->object->write('Content');
        $this->assertEquals('Content', $this->object->read());

        $this->object->append('Append');
        $this->assertEquals('ContentAppend', $this->object->read());

        $this->object->prepend('Prepend');
        $this->assertEquals('PrependContentAppend', $this->object->read());
    }

    public function testBasename(): void {
        $this->assertEquals('base.html', $this->object->basename());
        $this->assertEquals('temp.html', $this->temp->basename());
    }

    public function testCreate(): void {
        $this->assertTrue($this->object->exists());
        $this->assertFalse($this->object->create());

        $this->assertFalse($this->temp->exists());
        $this->assertTrue($this->temp->create());
        $this->assertTrue($this->temp->exists());
    }

    public function testExt(): void {
        $this->assertEquals('html', $this->object->ext());
        $this->assertEquals('html', $this->object->ext());

        $file = new File($this->vfs()->path('file/test'), true);

        $this->assertEquals('', $file->ext());
    }

    public function testLockAndUnlock(): void {
        $this->object->open('r');

        $this->assertTrue($this->object->lock());
        $this->assertFalse($this->temp->lock());

        $this->assertTrue($this->object->unlock());
        $this->assertFalse($this->temp->unlock());

        $this->object->close();
    }

    public function testMd5(): void {
        $this->assertEquals('d41d8cd98f00b204e9800998ecf8427e', $this->object->md5());
        $this->assertEquals(null, $this->temp->md5());

        $this->object->write('Change to content to produce a different hash.');
        $this->assertEquals('92dbea223f446b8d480a8fd4984232e4', $this->object->md5());
    }

    public function testMimeType(): void {
        $this->assertEquals('text/plain', (new File(TEMP_DIR . '/io/ini.ini'))->mimeType());
        $this->assertEquals('text/x-php', (new File(TEMP_DIR . '/io/php.php'))->mimeType());
        $this->assertEquals('application/xml', (new File(TEMP_DIR . '/io/xml.xml'))->mimeType());
    }

    public function testName(): void {
        $this->assertEquals('base', $this->object->name());
        $this->assertEquals('temp', $this->temp->name());
    }

    public function testOpenClose(): void {
        $this->assertFalse($this->object->close());
        $this->assertFalse($this->temp->close());

        $this->assertTrue($this->object->open('w'));
        $this->assertFalse($this->temp->open('w'));

        $this->assertTrue($this->object->close());
        $this->assertFalse($this->temp->close());
    }

    public function testPermissionReading(): void {
        $this->object->chmod(0777);
        $this->temp->chmod(0777);

        $this->assertTrue($this->object->executable());
        $this->assertTrue($this->object->readable());
        $this->assertTrue($this->object->writable());

        $this->assertFalse($this->temp->executable());
        $this->assertFalse($this->temp->readable());
        $this->assertFalse($this->temp->writable());

        $this->object->chmod(0);
        $this->temp->chmod(0);

        $this->assertFalse($this->object->executable());
        $this->assertFalse($this->object->readable());
        $this->assertFalse($this->object->writable());

        $this->assertFalse($this->temp->executable());
        $this->assertFalse($this->temp->readable());
        $this->assertFalse($this->temp->writable());
    }

    public function testReadWrite(): void {
        $content = 'Lets add a bit of fat to you before reading.';
        $this->object->write($content);

        $this->assertEquals($content, $this->object->read());
        $this->assertEquals('Lets add', $this->object->read(8));

        $this->object->write('');
        $this->assertEquals('', $this->object->read());
    }

    public function testSize(): void {
        $this->assertTrue($this->object->write('foobar'));
        $this->assertEquals(6, $this->object->size());
        $this->assertEquals(0, $this->temp->size());

        $this->assertTrue($this->object->write('You must weigh a ton.'));
        $this->assertEquals(21, $this->object->size());
    }

}
