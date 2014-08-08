<?hh
namespace Titon\Http\Server;

use Titon\Http\Http;
use Titon\Http\Stream\MemoryStream;
use Titon\Test\TestCase;
use Titon\Utility\Format;
use VirtualFileSystem\FileSystem;

/**
 * @property \Titon\Http\Server\Response $object
 */
class ResponseTest extends TestCase {

    protected $time;

    protected function setUp() {
        parent::setUp();

        $_SERVER['HTTP_ACCEPT_LANGUAGE'] = 'en-us,en;q=0.5';

        $this->time = time();
        $this->object = new Response(null, 200, Map {'debug' => true});
        $this->object->prepare(Request::createFromGlobals());
    }

    public function testAcceptRanges() {
        $this->object->acceptRanges();
        $this->assertEquals('bytes', $this->object->getHeader('Accept-Ranges'));

        $this->object->acceptRanges('none');
        $this->assertEquals('none', $this->object->getHeader('Accept-Ranges'));

        $this->object->acceptRanges('custom');
        $this->assertEquals('custom', $this->object->getHeader('Accept-Ranges'));
    }

    public function testAge() {
        $this->object->age(120);
        $this->assertEquals('120', $this->object->getHeader('Age'));

        $this->object->age('+1 hour');
        $this->assertEquals('3600', $this->object->getHeader('Age'));
    }

    public function testAllow() {
        $this->object->allow(Vector {'get'});
        $this->assertEquals('GET', $this->object->getHeader('Allow'));

        $this->object->allow(Vector {'POST', 'put', 'CUSTOM'});
        $this->assertEquals('POST, PUT', $this->object->getHeader('Allow'));
    }

    public function testCache() {
        $this->object->cache('none');
        $this->assertEquals('no-cache, no-store, must-revalidate, proxy-revalidate', $this->object->getHeader('Cache-Control'));

        $this->object->cache('none', 0, false);
        $this->assertEquals('no-cache, no-store, must-revalidate', $this->object->getHeader('Cache-Control'));

        $this->object->cache('public');
        $this->assertEquals('public, max-age=86400, s-maxage=86400', $this->object->getHeader('Cache-Control'));

        $this->object->cache('public', '+5 minutes', false);
        $this->assertEquals('public, max-age=300', $this->object->getHeader('Cache-Control'));

        $this->object->cache('private', '+1 hour');
        $this->assertEquals('private, max-age=3600, s-maxage=3600', $this->object->getHeader('Cache-Control'));

        $this->object->cache('private', '+1 hour', false);
        $this->assertEquals('private, max-age=3600', $this->object->getHeader('Cache-Control'));

        $this->object->cache('private', '+1 hour', false, ['no-transform']);
        $this->assertEquals('no-transform, private, max-age=3600', $this->object->getHeader('Cache-Control'));
    }

    /**
     * @expectedException \InvalidArgumentException
     */
    public function testCacheInvalidDirective() {
        $this->object->cache('foobar');
    }

    public function testCacheControl() {
        $this->object->cacheControl(['public']);
        $this->assertEquals('public', $this->object->getHeader('Cache-Control'));

        $this->object->cacheControl(['private', 'must-revalidate']);
        $this->assertEquals('private, must-revalidate', $this->object->getHeader('Cache-Control'));

        $this->object->cacheControl(['no-cache', 'max-age' => 0]);
        $this->assertEquals('no-cache, max-age=0', $this->object->getHeader('Cache-Control'));

        $this->object->cacheControl(['private' => 'foobar', 'max-age' => 123]);
        $this->assertEquals('private="foobar", max-age=123', $this->object->getHeader('Cache-Control'));
    }

    public function testConnection() {
        $this->object->connection(true);
        $this->assertEquals('keep-alive', $this->object->getHeader('Connection'));

        $this->object->connection(false);
        $this->assertEquals('close', $this->object->getHeader('Connection'));

        $this->object->connection('custom');
        $this->assertEquals('custom', $this->object->getHeader('Connection'));
    }

    public function testContentDisposition() {
        $this->object->contentDisposition('file.png');
        $this->assertEquals('attachment; filename="file.png"', $this->object->getHeader('Content-Disposition'));
    }

    /**
     * @expectedException \InvalidArgumentException
     */
    public function testContentDispositionInvalidType() {
        $this->object->contentDisposition('file.png', 'foobar');
    }

    public function testContentEncoding() {
        $this->object->contentEncoding('gzip');
        $this->assertEquals('gzip', $this->object->getHeader('Content-Encoding'));

        $this->object->contentEncoding('gzip, compress');
        $this->assertEquals('gzip, compress', $this->object->getHeader('Content-Encoding'));
    }

    public function testContentLanguage() {
        $this->object->contentLanguage('');
        $this->assertEquals('', $this->object->getHeader('Content-Language'));

        $this->object->contentLanguage('en, en-us');
        $this->assertEquals('en, en-us', $this->object->getHeader('Content-Language'));

        $this->object->contentLanguage('en, en-us');
        $this->assertEquals('en, en-us', $this->object->getHeader('Content-Language'));
    }

    public function testContentLocation() {
        $this->object->contentLocation('');
        $this->assertEquals('', $this->object->getHeader('Content-Location'));

        $this->object->contentLocation('/file.txt');
        $this->assertEquals('/file.txt', $this->object->getHeader('Content-Location'));
    }

    public function testContentLength() {
        $this->object->contentLength(1234);
        $this->assertEquals('1234', $this->object->getHeader('Content-Length'));

        $this->object->contentLength('2GB');
        $this->assertEquals('2147483648', $this->object->getHeader('Content-Length'));
    }

    public function testContentLength300Status() {
        $this->object->setStatusCode(302);
        $this->object->contentLength(1234);
        $this->assertEquals('', $this->object->getHeader('Content-Length'));
    }

    public function testContentMD5() {
        $this->object->contentMD5('AHASHHERE');
        $this->assertEquals('AHASHHERE', $this->object->getHeader('Content-MD5'));

        $this->object->body(new MemoryStream('body'))->contentMD5(false)->send();

        $this->object->contentMD5(true)->send();
        $this->assertEquals('hBotaJrYa9FhFEdFPCLG/A==', $this->object->getHeader('Content-MD5'));
    }

    public function testContentType() {
        $this->object->contentType('html');
        $this->assertEquals('text/html; charset=UTF-8', $this->object->getHeader('Content-Type'));

        $this->object->contentType('application/json');
        $this->assertEquals('application/json; charset=UTF-8', $this->object->getHeader('Content-Type'));

        $this->object->contentType('xhtml');
        $this->assertEquals('application/xhtml+xml', $this->object->getHeader('Content-Type'));
    }

    public function testContentType300Status() {
        $this->object->setStatusCode(304);
        $this->object->contentType('xhtml');
        $this->assertEquals('text/html; charset=UTF-8', $this->object->getHeader('Content-Type'));
    }

    /**
     * @expectedException \Titon\Http\Exception\InvalidExtensionException
     */
    public function testContentTypeInvalidType() {
        $this->object->contentType('fake');
    }

    public function testDate() {
        $this->object->date('+12 hours');
        $this->assertEquals(Format::http('+12 hours'), $this->object->getHeader('Date'));
    }

    public function testDownload() {
        $vfs = new FileSystem();
        $vfs->createFile('/download.txt');

        $this->assertInstanceOf('Titon\Http\Server\DownloadResponse', Response::download($vfs->path('/download.txt')));

        unset($vfs);
    }

    public function testEtag() {
        $this->object->etag('FooBar');
        $this->assertEquals('"FooBar"', $this->object->getHeader('ETag'));

        $this->object->etag('FooBar', true);
        $this->assertEquals('W/"FooBar"', $this->object->getHeader('ETag'));
    }

    public function testExpires() {
        $this->object->expires('+12 hours');
        $this->assertEquals(Format::http('+12 hours'), $this->object->getHeader('Expires'));
    }

    public function testGetHeaders() {
        // set by default
        $this->assertEquals([
            'Date' => [gmdate(Http::DATE_FORMAT, $this->time)],
            'Connection' => ['keep-alive'],
            'Content-Type' => ['text/html; charset=UTF-8'],
            'Status-Code' => ['200 OK']
        ], $this->object->getHeaders());

        $this->object->contentType('json');
        $this->assertEquals('application/json; charset=UTF-8', $this->object->getHeader('Content-Type'));

        $this->assertEquals([
            'Date' => [gmdate(Http::DATE_FORMAT, $this->time)],
            'Connection' => ['keep-alive'],
            'Content-Type' => ['application/json; charset=UTF-8'],
            'Status-Code' => ['200 OK']
        ], $this->object->getHeaders());
    }

    public function testGetProtocolVersion() {
        $this->assertEquals('1.1', $this->object->getProtocolVersion());
    }

    public function testGetProtocolVersionNoRequest() {
        $response = new Response();
        $this->assertEquals('1.1', $response->getProtocolVersion());
    }

    public function testGetReasonPhrase() {
        $this->object->setStatusCode(400);
        $this->assertEquals('Bad Request', $this->object->getReasonPhrase());
    }

    public function testHeader() {
        $this->object->setHeader('X-Framework', 'Titon');
        $this->assertEquals('Titon', $this->object->getHeader('X-Framework'));

        $this->object->setHeaders([
            'X-Framework' => 'Titon',
            'X-Version' => '1.2.3'
        ]);

        $this->assertEquals('Titon', $this->object->getHeader('X-Framework'));
        $this->assertEquals('1.2.3', $this->object->getHeader('X-Version'));
    }

    public function testJson() {
        $this->assertInstanceOf('Titon\Http\Server\JsonResponse', Response::json(['foo' => 'bar']));
    }

    public function testLastModified() {
        $this->object->lastModified('+12 hours');
        $this->assertEquals(Format::http('+12 hours'), $this->object->getHeader('Last-Modified'));
    }

    public function testLocation() {
        $this->object->location('/local/url');
        $this->assertEquals('/local/url', $this->object->getHeader('Location'));

        $this->object->location('http://google.com');
        $this->assertEquals('http://google.com', $this->object->getHeader('Location'));
    }

    public function testNoCache() {
        $this->object->noCache();
        $this->assertEquals(Format::http('-1 year'), $this->object->getHeader('Expires'));
        $this->assertEquals(Format::http(time()), $this->object->getHeader('Last-Modified'));
        $this->assertEquals('no-cache, no-store, must-revalidate, proxy-revalidate', $this->object->getHeader('Cache-Control'));
    }

    public function testNotModified() {
        $this->object->contentType('html')->notModified();
        $this->assertEquals([
            'Date' => [gmdate(Http::DATE_FORMAT, $this->time)],
            'Connection' => ['keep-alive'],
            'Status-Code' => ['304 Not Modified']
        ], $this->object->getHeaders());
    }

    public function testBodyAndRespond() {
        $this->object->body(new MemoryStream('<html><body>body</body></html>'));
        $this->assertEquals('<html><body>body</body></html>', $this->object->send());
    }

    public function testRedirect() {
        $this->assertInstanceOf('Titon\Http\Server\RedirectResponse', Response::redirect('/'));
    }

    public function testRemoveCookie() {
        $time = time();
        $this->object->removeCookie('foo');

        $this->assertEquals('foo=deleted; Expires=' . gmdate(Http::DATE_FORMAT, $time) . '; Path=/; HttpOnly', $this->object->getHeader('Set-Cookie'));
    }

    public function testRetryAfter() {
        $this->object->retryAfter(120);
        $this->assertEquals('120', $this->object->getHeader('Retry-After'));

        $this->object->retryAfter('+1 hour');
        $this->assertEquals(Format::http('+1 hour'), $this->object->getHeader('Retry-After'));
    }

    public function testSetCookie() {
        $time = strtotime('+1 week');
        $this->object->setCookie('foo', 'bar');
        $this->assertEquals('foo=WBQ%A3E%AA6%CD%E8%D9%CCC%E9%8D%01%8B; Expires=' . gmdate(Http::DATE_FORMAT, $time) . '; Path=/; HttpOnly', $this->object->getHeader('Set-Cookie'));
    }

    public function testSendBodyNoChunk() {
        $this->object->setConfig('buffer', false);
        $this->object->setBody(new MemoryStream('body'));

        ob_start();
        $this->object->sendBody();
        $body = ob_get_clean();

        $this->assertEquals('body', $body);
    }

    public function testStatusCode() {
        $this->object->statusCode(404);
        $this->assertEquals('404 Not Found', $this->object->getHeader('Status-Code'));

        $this->object->statusCode(500);
        $this->assertEquals('500 Internal Server Error', $this->object->getHeader('Status-Code'));
    }

    /**
     * @expectedException \Titon\Http\Exception\InvalidStatusException
     */
    public function testStatusCodeInvalidCode() {
        $this->object->statusCode(666);
    }

    public function testVary() {
        $this->object->vary('Accept');
        $this->assertEquals('Accept', $this->object->getHeader('Vary'));

        $this->object->vary('Accept, Cookie');
        $this->assertEquals('Accept, Cookie', $this->object->getHeader('Vary'));
    }

    public function testWwwAuthenticate() {
        $this->object->wwwAuthenticate('Basic');
        $this->assertEquals('Basic', $this->object->getHeader('WWW-Authenticate'));

        $this->object->wwwAuthenticate('Digest');
        $this->assertEquals('Digest', $this->object->getHeader('WWW-Authenticate'));
    }

    public function testXml() {
        $this->assertInstanceOf('Titon\Http\Server\XmlResponse', Response::xml(['foo' => 'bar']));
    }

}