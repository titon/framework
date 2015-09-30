<?hh
namespace Titon\Http;

use Titon\Http\Stream\MemoryStream;
use Titon\Test\Stub\Http\MessageStub;
use Titon\Test\TestCase;

/**
 * @property \Titon\Http\AbstractMessage $object
 */
class MessageTest extends TestCase {

    protected function setUp(): void {
        parent::setUp();

        $this->stream = new MemoryStream('base');
        $this->object = new MessageStub($this->stream);
        $this->object->getHeaderBag()->set('Content-Length', ['1337']);
    }

    public function testConstructorSetsHeaders(): void {
        $object = new MessageStub(null, Map {
            'Content-Type' => 'text/html',
            'Content-Length' => '125'
        });

        $this->assertEquals([
            'Content-Type' => ['text/html'],
            'Content-Length' => ['125']
        ], $object->getHeaders());
    }

    public function testConstructorCreatesStream(): void {
        $this->assertInstanceOf('Psr\Http\Message\StreamInterface', $this->object->getBody());
    }

    public function testBodyIsCloned(): void {
        $this->assertSame($this->object->getBody(), $this->object->getBody());

        $object = clone $this->object;

        $this->assertNotSame($object->getBody(), $this->object->getBody());
    }

    public function testNoBodyIsCreated(): void {
        $object = new MessageStub();

        $this->assertNotNull($object->getBody());
        $this->assertInstanceOf(MemoryStream::class, $object->getBody());
    }

    public function testBagsAreCloned(): void {
        $this->assertTrue($this->object->getHeaderBag() === $this->object->getHeaderBag());

        $object = clone $this->object;
        $object->getHeaderBag()->set('Content-Type', ['text/html']);

        $this->assertTrue($object->getHeaderBag() !== $this->object->getHeaderBag());
    }

    public function testGetHeader(): void {
        $this->assertEquals([], $this->object->getHeader('Content-Type'));

        $this->object->getHeaderBag()->set('Content-Type', ['text/html']);

        $this->assertEquals(['text/html'], $this->object->getHeader('Content-Type'));
    }

    public function testGetHeaderLine(): void {
        $this->assertEquals('', $this->object->getHeaderLine('Content-Type'));

        $this->object->getHeaderBag()->set('Content-Type', ['text/html', 'text/plain']);

        $this->assertEquals('text/html, text/plain', $this->object->getHeaderLine('Content-Type'));
    }

    public function testGetHeaders(): void {
        $this->assertEquals([
            'Content-Length' => ['1337']
        ], $this->object->getHeaders());

        $this->object->getHeaderBag()->set('Content-Type', ['text/html', 'text/plain']);

        $this->assertEquals([
            'Content-Length' => ['1337'],
            'Content-Type' => ['text/html', 'text/plain']
        ], $this->object->getHeaders());
    }

    public function testGetProtocolVersion(): void {
        $this->assertEquals('1.1', $this->object->getProtocolVersion());
    }

    public function testHasHeader(): void {
        $this->assertTrue($this->object->hasHeader('Content-Length'));
        $this->assertFalse($this->object->hasHeader('Content-Type'));
    }

    public function testWithAddedHeader(): void {
        $this->object->getHeaderBag()->set('Content-Type', ['text/html']);

        $object = $this->object->withAddedHeader('Content-Type', 'text/plain');

        $this->assertNotSame($object, $this->object);

        $this->assertEquals([
            'Content-Length' => ['1337'],
            'Content-Type' => ['text/html']
        ], $this->object->getHeaders());

        $this->assertEquals([
            'Content-Length' => ['1337'],
            'Content-Type' => ['text/html', 'text/plain']
        ], $object->getHeaders());
    }

    public function testWithAddedHeaderArray(): void {
        $object = $this->object->withAddedHeader('Content-Type', ['text/html', 'text/plain']);

        $this->assertNotSame($object, $this->object);

        $this->assertEquals([
            'Content-Length' => ['1337']
        ], $this->object->getHeaders());

        $this->assertEquals([
            'Content-Length' => ['1337'],
            'Content-Type' => ['text/html', 'text/plain']
        ], $object->getHeaders());
    }

    public function testWithBody(): void {
        $object = $this->object->withBody(new MemoryStream('clone'));

        $this->assertNotSame($object, $this->object);
        $this->assertNotSame($object->getBody(), $this->object->getBody());
    }

    public function testWithBodySameStream(): void {
        $object = $this->object->withBody($this->stream);

        $this->assertSame($object, $this->object);
        $this->assertSame($object->getBody(), $this->object->getBody());
    }

    public function testWithHeader(): void {
        $this->object->getHeaderBag()->set('Content-Type', ['text/html']);

        $object = $this->object->withHeader('Content-Type', 'text/plain');

        $this->assertNotSame($object, $this->object);

        $this->assertEquals([
            'Content-Length' => ['1337'],
            'Content-Type' => ['text/html']
        ], $this->object->getHeaders());

        $this->assertEquals([
            'Content-Length' => ['1337'],
            'Content-Type' => ['text/plain']
        ], $object->getHeaders());
    }

    public function testWithHeaderArray(): void {
        $this->object->getHeaderBag()->set('Content-Type', ['text/html']);

        $object = $this->object->withHeader('Content-Type', ['application/json', 'text/javascript']);

        $this->assertNotSame($object, $this->object);

        $this->assertEquals([
            'Content-Length' => ['1337'],
            'Content-Type' => ['text/html']
        ], $this->object->getHeaders());

        $this->assertEquals([
            'Content-Length' => ['1337'],
            'Content-Type' => ['application/json', 'text/javascript']
        ], $object->getHeaders());
    }

    public function testWithProtocolVersion(): void {
        $object = $this->object->withProtocolVersion('2.0');

        $this->assertNotSame($object, $this->object);

        $this->assertEquals('1.1', $this->object->getProtocolVersion());
        $this->assertEquals('2.0', $object->getProtocolVersion());
    }

    public function testWithoutHeader(): void {
        $object = $this->object->withoutHeader('Content-Length');

        $this->assertNotSame($object, $this->object);

        $this->assertEquals([
            'Content-Length' => ['1337']
        ], $this->object->getHeaders());

        $this->assertEquals([], $object->getHeaders());
    }

    public function testWithoutHeaderNoHeaderByName(): void {
        $object = $this->object->withoutHeader('Content-Type');

        $this->assertSame($object, $this->object);
    }
}
