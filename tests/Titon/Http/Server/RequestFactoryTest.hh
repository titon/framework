<?hh
namespace Titon\Http\Server;

use Titon\Http\Uri;
use Titon\Test\TestCase;

class RequestFactoryTest extends TestCase {

    public function testCreate(): void {
        $request = RequestFactory::create(Map {}, '/', 'GET', Map {
            'Content-Type' => ['text/plain']
        });

        $this->assertInstanceOf(Request::class, $request);
        $this->assertEquals('/', (string) $request->getUri());
        $this->assertEquals('GET', $request->getMethod());
        $this->assertEquals('text/plain', $request->getHeaderLine('Content-Type'));
    }

    public function testCreateUrlAsObject(): void {
        $uri = new Uri('http://titon.io');
        $request = RequestFactory::create(Map {}, $uri);

        $this->assertInstanceOf(Request::class, $request);
        $this->assertEquals('http://titon.io', (string) $request->getUri());
    }

    public function testCreateExtractsServerHeaders(): void {
        $request = RequestFactory::create(Map {
            'HTTP_ACCEPT' => 'text/html'
        }, '/foo', 'POST');

        $this->assertInstanceOf(Request::class, $request);
        $this->assertEquals('/foo', (string) $request->getUri());
        $this->assertEquals('POST', $request->getMethod());
        $this->assertEquals('text/html', $request->getHeaderLine('Accept'));
    }

    public function testCreateCustomHeadersOverrideServerHeaders(): void {
        $request = RequestFactory::create(Map {
            'HTTP_ACCEPT' => 'text/html'
        }, '/bar', 'PUT', Map {
            'Accept' => ['text/plain']
        });

        $this->assertInstanceOf(Request::class, $request);
        $this->assertEquals('/bar', (string) $request->getUri());
        $this->assertEquals('PUT', $request->getMethod());
        $this->assertEquals('text/plain', $request->getHeaderLine('Accept'));
    }

    public function testExtractHeaders(): void {
        $headers = RequestFactory::extractHeaders(Map {
            'CONTENT_TYPE' => 'text/html',
            'HTTP_HOST' => 'titon.io',
            'HTTP_USER_AGENT' => 'chrome',
            'HTTP_COOKIE' => 'foo=bar',
            'SCRIPT_FILENAME' => 'index.php',
            'PHP_SELF' => ''
        });

        $this->assertEquals(Map {
            'CONTENT_TYPE' => 'text/html',
            'HOST' => 'titon.io',
            'USER_AGENT' => 'chrome',
        }, $headers);
    }
}
