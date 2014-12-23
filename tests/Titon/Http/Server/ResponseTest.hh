<?hh
namespace Titon\Http\Server;

use Titon\Http\Http;
use Titon\Http\Stream\MemoryStream;
use Titon\Test\TestCase;
use Titon\Utility\Format;

/**
 * @property \Titon\Http\Server\Response $object
 */
class ResponseTest extends TestCase {

    protected $time;

    protected function setUp() {
        parent::setUp();

        $_SERVER['HTTP_ACCEPT_LANGUAGE'] = 'en-us,en;q=0.5';

        $this->time = time();

        $this->object = new Response();
        $this->object->debug();
    }

    public function testAcceptRanges() {
        $this->object->acceptRanges();
        $this->assertEquals('bytes', $this->object->getHeader('Accept-Ranges'));

        $this->object->acceptRanges('none');
        $this->assertEquals('none', $this->object->getHeader('Accept-Ranges'));

        $this->object->acceptRanges('custom');
        $this->assertEquals('custom', $this->object->getHeader('Accept-Ranges'));
    }

    public function testAddHeader() {
        $this->object->addHeader('Accept-Charset', 'utf-8');
        $this->object->addHeader('Accept-Charset', 'utf-16');

        $this->assertEquals(['utf-8', 'utf-16'], $this->object->getHeaderAsArray('Accept-Charset'));
    }

    public function testAddHeaders() {
        $this->object->addHeader('Accept-Charset', 'utf-8');
        $this->object->addHeader('Accept-Language', 'en');
        $this->object->addHeaders([
            'Accept-Charset' => 'utf-16',
            'Accept-Language' => 'fr'
        ]);

        $this->assertEquals(['utf-8', 'utf-16'], $this->object->getHeaderAsArray('Accept-Charset'));
        $this->assertEquals(['en', 'fr'], $this->object->getHeaderAsArray('Accept-Language'));
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

        $this->object->cache('private', '+1 hour', false, Map {'no-transform' => true});
        $this->assertEquals('private, max-age=3600, no-transform', $this->object->getHeader('Cache-Control'));
    }

    /**
     * @expectedException \InvalidArgumentException
     */
    public function testCacheInvalidDirective() {
        $this->object->cache('foobar');
    }

    public function testCacheControl() {
        $this->object->cacheControl(Map {'public' => true});
        $this->assertEquals('public', $this->object->getHeader('Cache-Control'));

        $this->object->cacheControl(Map {'private' => true, 'must-revalidate' => true});
        $this->assertEquals('private, must-revalidate', $this->object->getHeader('Cache-Control'));

        $this->object->cacheControl(Map {'no-cache' => true, 'max-age' => 0});
        $this->assertEquals('no-cache, max-age=0', $this->object->getHeader('Cache-Control'));

        $this->object->cacheControl(Map {'private' => 'foobar', 'max-age' => 123});
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

        $this->object->contentDisposition('file.png', 'inline');
        $this->assertEquals('inline; filename="file.png"', $this->object->getHeader('Content-Disposition'));
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
        $this->object->setStatus(302);
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

    public function testContentRange() {
        $this->object->contentRange(0, 50, 100);
        $this->assertEquals('bytes 0-50/100', $this->object->getHeader('Content-Range'));
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
        $this->object->setStatus(304);
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
        $this->setupVFS();
        $this->vfs->createFile('/download.txt');

        $this->assertInstanceOf('Titon\Http\Server\DownloadResponse', Response::download($this->vfs->path('/download.txt')));
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
        $this->assertEquals([
            'Date' => [gmdate(Http::DATE_FORMAT, $this->time)],
            'Connection' => ['keep-alive'],
            'Content-Type' => ['text/html; charset=UTF-8'],
            'Status-Code' => ['200 OK']
        ], $this->object->getHeaders()); // Set by default

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

    public function testGetReasonPhrase() {
        $this->object->setStatus(400);
        $this->assertEquals('Bad Request', $this->object->getReasonPhrase());
    }

    public function testGetStatusCode() {
        $this->assertEquals(200, $this->object->getStatusCode());
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

    public function testRedirect() {
        $this->assertInstanceOf('Titon\Http\Server\RedirectResponse', Response::redirect('/'));
    }

    public function testRemoveCookie() {
        $time = time();
        $this->object->removeCookie('foo');

        $this->assertEquals('foo=deleted; Expires=' . gmdate(Http::DATE_FORMAT, $time) . '; Path=/; HttpOnly', $this->object->getHeader('Set-Cookie'));
    }

    public function testRemoveHeader() {
        $this->object->setHeader('Content-Type', 'text/html');
        $this->assertTrue($this->object->hasHeader('Content-Type'));

        $this->object->removeHeader('Content-Type');
        $this->assertFalse($this->object->hasHeader('Content-Type'));
    }

    public function testRemoveHeaders() {
        $this->object->setHeader('Content-Type', 'text/html');
        $this->object->setHeader('Content-Length', 100);

        $this->assertEquals([
            'Content-Type' => ['text/html'],
            'Content-Length' => [100],
            'Date' => [gmdate(Http::DATE_FORMAT, $this->time)],
            'Connection' => ['keep-alive'],
            'Status-Code' => ['200 OK']
        ], $this->object->getHeaders());

        $this->object->removeHeaders(['Content-Type', 'Content-Length', 'Date', 'Connection']);

        $this->assertEquals([
            'Status-Code' => ['200 OK']
        ], $this->object->getHeaders());
    }

    public function testRetryAfter() {
        $this->object->retryAfter(120);
        $this->assertEquals('120', $this->object->getHeader('Retry-After'));

        $this->object->retryAfter('+1 hour');
        $this->assertEquals(Format::http('+1 hour'), $this->object->getHeader('Retry-After'));
    }

    public function testSendBody() {
        $this->object->setBody(new MemoryStream('body'));

        ob_start();
        $this->object->sendBody();
        $body = ob_get_clean();

        $this->assertEquals('body', $body);
    }

    public function testSendBodyAndHeaders() {
        $this->object->body(new MemoryStream('<html><body>body</body></html>'));
        $this->assertEquals('<html><body>body</body></html>', $this->object->send());
    }

    public function testSetHeader() {
        $this->object->setHeader('X-Framework', 'Titon');
        $this->assertEquals('Titon', $this->object->getHeader('X-Framework'));
    }

    public function testSetHeaders() {
        $this->object->setHeaders([
            'X-Framework' => 'Titon',
            'X-Version' => '1.2.3'
        ]);

        $this->assertEquals('Titon', $this->object->getHeader('X-Framework'));
        $this->assertEquals('1.2.3', $this->object->getHeader('X-Version'));
    }

    public function testSetCookie() {
        $time = strtotime('+1 week');
        $this->object->setCookie('foo', 'bar', $time);
        $this->assertEquals('foo=5hxAThObwiiTyh0mhfxIKw%3D%3D; Expires=' . gmdate(Http::DATE_FORMAT, $time) . '; Path=/; HttpOnly', $this->object->getHeader('Set-Cookie'));
    }

    public function testSetProtocolVersion() {
        $this->object->setProtocolVersion('1.0');
        $this->assertEquals('1.0', $this->object->getProtocolVersion());
    }

    public function testSetReasonPhrase() {
        $this->object->setReasonPhrase('Foo Bar');
        $this->assertEquals('Foo Bar', $this->object->getReasonPhrase());
    }

    public function testStatus() {
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