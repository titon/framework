<?hh
namespace Titon\Http;

use Psr\Http\Message\UriInterface;
use Titon\Test\Stub\Http\RequestStub;
use Titon\Test\TestCase;

/**
 * @property \Titon\Http\AbstractRequest $object
 */
class RequestTest extends TestCase {

    protected function setUp(): void {
        parent::setUp();

        $this->object = new RequestStub(new Uri('/'));
    }

    public function testConstructorNoHost(): void {
        $this->assertEquals('', $this->object->getHeaderLine('Host'));
    }

    public function testConstructorSetsHost(): void {
        $uri = new Uri('http://titon.io/framework');
        $object = new RequestStub($uri);

        $this->assertSame($uri, $object->getUri());
        $this->assertEquals('titon.io', $object->getHeaderLine('Host'));
    }

    /**
     * @expectedException \Titon\Http\Exception\InvalidMethodException
     */
    public function testConstructorValidatesMethod(): void {
        new RequestStub(new Uri('/'), 'FOO');
    }

    public function testGetMethod(): void {
        $this->assertEquals('GET', $this->object->getMethod());
    }

    public function testGetRequestTarget(): void {
        $this->assertEquals('/', $this->object->getRequestTarget());
    }

    public function testGetRequestTargetWithPath(): void {
        $object = new RequestStub(new Uri('http://titon.io/framework'));

        $this->assertEquals('/framework', $object->getRequestTarget());
    }

    public function testGetRequestTargetWithQuery(): void {
        $object = new RequestStub(new Uri('http://titon.io?foo=bar'));

        $this->assertEquals('/?foo=bar', $object->getRequestTarget());
    }

    public function testGetRequestTargetWithPathAndQuery(): void {
        $object = new RequestStub(new Uri('http://titon.io/framework?foo=bar'));

        $this->assertEquals('/framework?foo=bar', $object->getRequestTarget());
    }

    public function testGetUri(): void {
        $this->assertInstanceOf(UriInterface::class, $this->object->getUri());
    }

    public function testWithMethod(): void {
        $object = $this->object->withMethod('POST');

        $this->assertNotSame($this->object, $object);

        $this->assertEquals('GET', $this->object->getMethod());
        $this->assertEquals('POST', $object->getMethod());
    }

    /**
     * @expectedException \Titon\Http\Exception\InvalidMethodException
     */
    public function testWithMethodValidatesMethod(): void {
        $this->object->withMethod('BAR');
    }

    public function testWithRequestTarget(): void {
        $object = $this->object->withRequestTarget('/toolkit');

        $this->assertNotSame($this->object, $object);

        $this->assertEquals('/', $this->object->getRequestTarget());
        $this->assertEquals('/toolkit', $object->getRequestTarget());
    }

    public function testWithUri(): void {
        $object = $this->object->withUri(new Uri('http://titon.io'));

        $this->assertNotSame($this->object, $object);

        $this->assertEquals('', $this->object->getHeaderLine('Host'));
        $this->assertEquals('titon.io', $object->getHeaderLine('Host'));
    }

    public function testWithUriPreserveHost(): void {
        $this->object->getHeaderBag()->set('Host', ['titon.com']);

        $object = $this->object->withUri(new Uri('http://titon.io'), true);

        $this->assertNotSame($this->object, $object);

        $this->assertEquals('titon.com', $this->object->getHeaderLine('Host'));
        $this->assertEquals('titon.com', $object->getHeaderLine('Host'));
    }

}
