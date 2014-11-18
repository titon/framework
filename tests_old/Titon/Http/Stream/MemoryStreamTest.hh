<?hh
namespace Titon\Http\Stream;

use Titon\Test\TestCase;

/**
 * @property \Titon\Http\Stream\MemoryStream $object
 */
class MemoryStreamTest extends TestCase {

    protected function setUp() {
        parent::setUp();

        $this->object = new MemoryStream('foo');
    }

    protected function tearDown() {
        parent::tearDown();

        $this->object->close();
    }

    public function testToString() {
        $this->assertEquals('foo', (string) $this->object);
    }

    public function testToStringRestoresSeek() {
        $this->object->seek(1);

        $this->assertEquals(1, $this->object->tell());
        $this->assertEquals('foo', (string) $this->object);
        $this->assertEquals(1, $this->object->tell());
    }

    public function testClose() {
        $this->assertTrue($this->object->close());
        $this->assertFalse($this->object->close());
    }

    public function testDetach() {
        $this->assertTrue(is_resource($this->object->getStream()));
        $this->object->detach();
        $this->assertEquals(null, $this->object->getStream());
    }

    public function testEof() {
        $this->assertFalse($this->object->eof());
        $this->object->read(3);
        $this->assertTrue($this->object->eof());
    }

    public function testGetContents() {
        $this->assertEquals('foo', $this->object->getContents());
        $this->assertEquals('fo', $this->object->getContents(2));
    }

    public function testGetSize() {
        $this->assertEquals(3, $this->object->getSize());
    }

    public function testSizeIsConsistent() {
        $this->assertEquals(3, $this->object->getSize());
        $this->object->write('bar');
        $this->assertEquals(6, $this->object->getSize());
    }

    public function testRead() {
        $this->object->rewind();
        $this->assertEquals('fo', $this->object->read(2));

        $this->assertEquals('o', $this->object->read(3));

        $this->object->rewind();
        $this->assertEquals('foo', $this->object->read(5));
    }

    public function testSeekAndTell() {
        $this->assertEquals(3, $this->object->tell());

        $this->object->seek(1);
        $this->assertEquals(1, $this->object->tell());

        $this->object->seek(5);
        $this->assertEquals(5, $this->object->tell());
    }

    public function testWrite() {
        $this->assertEquals('foo', $this->object->getContents());
        $this->object->write('bar');
        $this->assertEquals('foobar', $this->object->getContents());
    }

}