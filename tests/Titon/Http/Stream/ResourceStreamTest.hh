<?hh
namespace Titon\Http\Stream;

use Titon\Test\TestCase;

class ResourceStreamTest extends TestCase {

    protected function makeStream(string $mode, string $content = ''): ResourceStream {
        $stream = new ResourceStream(fopen('php://memory', $mode));

        if ($content) {
            $stream->write($content);
        }

        return $stream;
    }

    public function testBuildCacheReadable(): void {
        $this->assertTrue($this->makeStream('r')->isReadable());
        $this->assertFalse($this->makeStream('w')->isReadable());
        $this->assertFalse($this->makeStream('a')->isReadable());
        $this->assertFalse($this->makeStream('xb')->isReadable());
    }

    public function testBuildCacheWritable(): void {
        $this->assertTrue($this->makeStream('w')->isWritable());
        $this->assertTrue($this->makeStream('w+')->isWritable());
        $this->assertTrue($this->makeStream('w+b')->isWritable());
        $this->assertFalse($this->makeStream('r')->isWritable());
    }

    public function testCloseOnDestruct(): void {
        $handle = fopen('php://memory', 'r');
        $stream = new ResourceStream($handle);
        unset($stream);
        $this->assertFalse(is_resource($handle));
    }

    /**
     * @expectedException \RuntimeException
     */
    public function testGetContentsThrowsErrorNotReadable(): void {
        $this->makeStream('w', 'foo')->getContents();
    }

    /**
     * @expectedException \RuntimeException
     */
    public function testReadThrowsErrorNonReadable(): void {
        $this->makeStream('w', 'foo')->read(3);
    }

    /**
     * @expectedException \RuntimeException
     */
    public function testWriteThrowsErrorNonWritable(): void {
        $this->makeStream('r')->write('foo');
    }

}
