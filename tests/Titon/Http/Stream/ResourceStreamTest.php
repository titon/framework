<?hh
namespace Titon\Http\Stream;

use Titon\Test\TestCase;

class ResourceStreamTest extends TestCase {

    protected function makeStream($mode, $content = '') {
        $stream = new ResourceStream(fopen('php://memory', $mode));

        if ($content) {
            $stream->write($content);
        }

        return $stream;
    }

    public function testBuildCacheReadable() {
        $this->assertTrue($this->makeStream('r')->isReadable());
        $this->assertFalse($this->makeStream('w')->isReadable());
        $this->assertFalse($this->makeStream('a')->isReadable());
        $this->assertFalse($this->makeStream('xb')->isReadable());
    }

    public function testBuildCacheWritable() {
        $this->assertTrue($this->makeStream('w')->isWritable());
        $this->assertTrue($this->makeStream('w+')->isWritable());
        $this->assertTrue($this->makeStream('w+b')->isWritable());
        $this->assertFalse($this->makeStream('r')->isWritable());
    }

    public function testCloseOnDestruct() {
        $handle = fopen('php://memory', 'r');
        $stream = new ResourceStream($handle);
        unset($stream);
        $this->assertFalse(is_resource($handle));
    }

    public function testGetContentsNotReadableReturnsEmpty() {
        $this->assertEquals('', $this->makeStream('w', 'foo')->getContents());
    }

    public function testReadReturnsNullNonReadable() {
        $this->assertEquals(null, $this->makeStream('w', 'foo')->read(3));
    }

    public function testWriteReturnsNullNonWritable() {
        $this->assertEquals(null, $this->makeStream('r')->write('foo'));
    }

}