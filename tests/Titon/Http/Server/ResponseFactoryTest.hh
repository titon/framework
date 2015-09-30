<?hh
namespace Titon\Http\Server;

use Titon\Http\StatusCode;
use Titon\Http\Stream\MemoryStream;
use Titon\Http\Stream\ResourceStream;
use Titon\Test\TestCase;

class ResponseFactoryTest extends TestCase {

    protected function setUp(): void {
        parent::setUp();

        $this->vfs()->createDirectory('/http/');
        $this->vfs()->createFile('/http/download.txt', 'This will be downloaded! Let\'s fluff this file with even more data to increase the file size.');
    }

    public function testCreate(): void {
        $response = ResponseFactory::create(new MemoryStream('foo'), StatusCode::METHOD_NOT_ALLOWED, Map {
            'Content-Type' => ['text/plain']
        });

        $this->assertInstanceOf(Response::class, $response);
        $this->assertEquals(405, $response->getStatusCode());
        $this->assertEquals('text/plain', $response->getHeaderLine('Content-Type'));
        $this->assertEquals('foo', $response->getBody()->getContents());
    }

    public function testCreateFromString(): void {
        $response = ResponseFactory::create('foo', StatusCode::NOT_FOUND);

        $this->assertInstanceOf(Response::class, $response);
        $this->assertInstanceOf(MemoryStream::class, $response->getBody());
        $this->assertEquals(404, $response->getStatusCode());
        $this->assertEquals('foo', $response->getBody()->getContents());
    }

    public function testCreateFromResource(): void {
        $response = ResponseFactory::create(fopen($this->vfs()->path('/http/download.txt'), 'r'));

        $this->assertInstanceOf(Response::class, $response);
        $this->assertInstanceOf(ResourceStream::class, $response->getBody());
        $this->assertEquals(200, $response->getStatusCode());
        $this->assertEquals('This will be downloaded! Let\'s fluff this file with even more data to increase the file size.', $response->getBody()->getContents());
    }

    /**
     * @expectedException \InvalidArgumentException
     */
    public function testCreateInvalidData(): void {
        ResponseFactory::create(true);
    }

    public function testDownload(): void {
        $this->markTestIncomplete('TO DO');
    }

    public function testJson(): void {
        $response = ResponseFactory::json(['foo' => 'bar']);

        $this->assertInstanceOf(JsonResponse::class, $response);
        $this->assertEquals(200, $response->getStatusCode());
        $this->assertEquals('application/json; charset=UTF-8', $response->getHeaderLine('Content-Type'));
        $this->assertEquals('{"foo":"bar"}', $response->getBody()->getContents());
    }

    public function testJsonCallbackJsonp(): void {
        $response = ResponseFactory::json(['foo' => 'bar'], StatusCode::BAD_REQUEST, -1, 'Titon.jsonp');

        $this->assertInstanceOf(JsonResponse::class, $response);
        $this->assertEquals(400, $response->getStatusCode());
        $this->assertEquals('text/javascript; charset=UTF-8', $response->getHeaderLine('Content-Type'));
        $this->assertEquals('Titon.jsonp({"foo":"bar"});', $response->getBody()->getContents());
        $this->assertEquals('Titon.jsonp', $response->getCallback());
    }

    public function testRedirect(): void {
        $response = ResponseFactory::redirect('/to/url');

        $this->assertInstanceOf(RedirectResponse::class, $response);
        $this->assertEquals(302, $response->getStatusCode());
        $this->assertEquals('/to/url', $response->getHeaderLine('Location'));
    }

    public function testRedirectBack(): void {
        $this->markTestIncomplete('TO DO');
    }

    public function testRedirectHome(): void {
        $this->markTestIncomplete('TO DO');
    }

    public function testRefresh(): void {
        $this->markTestIncomplete('TO DO');
    }

    public function testXml(): void {
        $response = ResponseFactory::xml(['foo' => 'bar']);

        $this->assertInstanceOf(XmlResponse::class, $response);
        $this->assertEquals(200, $response->getStatusCode());
        $this->assertEquals('application/xml; charset=UTF-8', $response->getHeaderLine('Content-Type'));
        $this->assertEquals(
            '<?xml version="1.0" encoding="UTF-8"?>' . PHP_EOL .
            '<root>' . PHP_EOL .
            '    <foo>bar</foo>' . PHP_EOL .
            '</root>' . PHP_EOL
        , $response->getBody()->getContents());
    }
}
