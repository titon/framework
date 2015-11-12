<?hh
namespace Titon\Http\Stream;

use Titon\Test\TestCase;

/**
 * @property \Titon\Http\Stream\MemoryStream $object
 */
class MemoryStreamTest extends TestCase {

    protected function setUp(): void {
        parent::setUp();

        $this->object = new MemoryStream('foo');
    }

    protected function tearDown(): void {
        parent::tearDown();

        $this->object->close();
    }

    public function testToString(): void {
        $this->assertEquals('foo', (string) $this->object);
    }

    public function testToStringRestoresSeek(): void {
        $this->object->seek(1);

        $this->assertEquals(1, $this->object->tell());
        $this->assertEquals('foo', (string) $this->object);
        $this->assertEquals(1, $this->object->tell());
    }

    public function testClose(): void {
        $this->object->close();
        $this->assertNull($this->object->getStream());
    }

    public function testDetach(): void {
        $this->assertTrue(is_resource($this->object->getStream()));
        $this->object->detach();
        $this->assertNull($this->object->getStream());
    }

    public function testEof(): void {
        $this->assertFalse($this->object->eof());
        $this->object->read(3);
        $this->assertTrue($this->object->eof());
    }

    public function testGetContents(): void {
        $this->assertEquals('foo', $this->object->getContents());
    }

    public function testGetMetaData(): void {
        $this->assertTrue($this->object->getMetadata('readable'));
        $this->assertTrue(is_array($this->object->getMetadata()));
    }

    public function testGetSize(): void {
        $this->assertEquals(3, $this->object->getSize());
    }

    public function testSizeIsConsistent(): void {
        $this->assertEquals(3, $this->object->getSize());
        $this->object->write('bar');
        $this->assertEquals(6, $this->object->getSize());
    }

    public function testRead(): void {
        $this->object->rewind();
        $this->assertEquals('fo', $this->object->read(2));
        $this->assertEquals('o', $this->object->read(3));

        $this->object->rewind();
        $this->assertEquals('foo', $this->object->read(5));
    }

    public function testSeekAndTell(): void {
        $this->assertEquals(3, $this->object->tell());

        $this->object->seek(1);
        $this->assertEquals(1, $this->object->tell());

        $this->object->seek(5);
        $this->assertEquals(5, $this->object->tell());
    }

    public function testWrite(): void {
        $this->assertEquals('foo', $this->object->getContents());
        $this->object->write('bar');
        $this->assertEquals('foobar', $this->object->getContents());
    }

}
