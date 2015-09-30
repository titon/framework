<?hh
namespace Titon\Http\Server;

use Titon\Http\Http;
use Titon\Test\TestCase;
use Titon\Utility\Format;

/**
 * @property \Titon\Http\Server\Response $object
 */
class ResponseTest extends TestCase {

    protected function setUp(): void {
        parent::setUp();

        $this->object = new Response();
    }

    public function testAcceptRanges(): void {
        $object = $this->object->acceptRanges();
        $this->assertEquals('bytes', $object->getHeaderLine('Accept-Ranges'));

        $object = $this->object->acceptRanges('none');
        $this->assertEquals('none', $object->getHeaderLine('Accept-Ranges'));

        $object = $this->object->acceptRanges('custom');
        $this->assertEquals('custom', $object->getHeaderLine('Accept-Ranges'));

        $this->assertNotSame($object, $this->object);
    }

    public function testAge(): void {
        $object = $this->object->age(120);
        $this->assertEquals('120', $object->getHeaderLine('Age'));

        $object = $this->object->age('+1 hour');
        $this->assertEquals('3600', $object->getHeaderLine('Age'));

        $this->assertNotSame($object, $this->object);
    }

    public function testAllow(): void {
        $object = $this->object->allow(Set {'get'});
        $this->assertEquals('GET', $object->getHeaderLine('Allow'));

        $object = $this->object->allow(Set {'POST', 'put', 'CUSTOM'});
        $this->assertEquals('POST, PUT', $object->getHeaderLine('Allow'));

        $this->assertNotSame($object, $this->object);
    }

    public function testCache(): void {
        $object = $this->object->cache('none');
        $this->assertEquals('no-cache, no-store, must-revalidate, proxy-revalidate', $object->getHeaderLine('Cache-Control'));

        $object = $this->object->cache('none', 0, false);
        $this->assertEquals('no-cache, no-store, must-revalidate', $object->getHeaderLine('Cache-Control'));

        $object = $this->object->cache('public');
        $this->assertEquals('public, max-age=86400, s-maxage=86400', $object->getHeaderLine('Cache-Control'));

        $object = $this->object->cache('public', '+5 minutes', false);
        $this->assertEquals('public, max-age=300', $object->getHeaderLine('Cache-Control'));

        $object = $this->object->cache('private', '+1 hour');
        $this->assertEquals('private, max-age=3600, s-maxage=3600', $object->getHeaderLine('Cache-Control'));

        $object = $this->object->cache('private', '+1 hour', false);
        $this->assertEquals('private, max-age=3600', $object->getHeaderLine('Cache-Control'));

        $object = $this->object->cache('private', '+1 hour', false, Map {'no-transform' => true});
        $this->assertEquals('private, max-age=3600, no-transform', $object->getHeaderLine('Cache-Control'));

        $this->assertNotSame($object, $this->object);
    }

    /**
     * @expectedException \InvalidArgumentException
     */
    public function testCacheInvalidDirective(): void {
        $this->object->cache('foobar');
    }

    public function testCacheControl(): void {
        $object = $this->object->cacheControl(Map {'public' => true});
        $this->assertEquals('public', $object->getHeaderLine('Cache-Control'));

        $object = $this->object->cacheControl(Map {'private' => true, 'must-revalidate' => true});
        $this->assertEquals('private, must-revalidate', $object->getHeaderLine('Cache-Control'));

        $object = $this->object->cacheControl(Map {'no-cache' => true, 'max-age' => 0});
        $this->assertEquals('no-cache, max-age=0', $object->getHeaderLine('Cache-Control'));

        $object = $this->object->cacheControl(Map {'private' => 'foobar', 'max-age' => 123});
        $this->assertEquals('private="foobar", max-age=123', $object->getHeaderLine('Cache-Control'));

        $this->assertNotSame($object, $this->object);
    }

    public function testConnection(): void {
        $object = $this->object->connection(true);
        $this->assertEquals('keep-alive', $object->getHeaderLine('Connection'));

        $object = $this->object->connection(false);
        $this->assertEquals('close', $object->getHeaderLine('Connection'));

        $object = $this->object->connection('custom');
        $this->assertEquals('custom', $object->getHeaderLine('Connection'));

        $this->assertNotSame($object, $this->object);
    }

    public function testContentDisposition(): void {
        $object = $this->object->contentDisposition('file.png');
        $this->assertEquals('attachment; filename="file.png"', $object->getHeaderLine('Content-Disposition'));

        $object = $this->object->contentDisposition('file.png', 'inline');
        $this->assertEquals('inline; filename="file.png"', $object->getHeaderLine('Content-Disposition'));

        $this->assertNotSame($object, $this->object);
    }

    /**
     * @expectedException \InvalidArgumentException
     */
    public function testContentDispositionInvalidType(): void {
        $this->object->contentDisposition('file.png', 'foobar');
    }

    public function testContentEncoding(): void {
        $object = $this->object->contentEncoding('gzip');
        $this->assertEquals('gzip', $object->getHeaderLine('Content-Encoding'));

        $object = $this->object->contentEncoding('gzip, compress');
        $this->assertEquals('gzip, compress', $object->getHeaderLine('Content-Encoding'));

        $this->assertNotSame($object, $this->object);
    }

    public function testContentLanguage(): void {
        $object = $this->object->contentLanguage('');
        $this->assertEquals('', $object->getHeaderLine('Content-Language'));

        $object = $this->object->contentLanguage('en, en-us');
        $this->assertEquals('en, en-us', $object->getHeaderLine('Content-Language'));

        $object = $this->object->contentLanguage('en, en-us');
        $this->assertEquals('en, en-us', $object->getHeaderLine('Content-Language'));

        $this->assertNotSame($object, $this->object);
    }

    public function testContentLocation(): void {
        $object = $this->object->contentLocation('');
        $this->assertEquals('', $object->getHeaderLine('Content-Location'));

        $object = $this->object->contentLocation('/file.txt');
        $this->assertEquals('/file.txt', $object->getHeaderLine('Content-Location'));

        $this->assertNotSame($object, $this->object);
    }

    public function testContentLength(): void {
        $object = $this->object->contentLength(1234);
        $this->assertEquals('1234', $object->getHeaderLine('Content-Length'));

        $object = $this->object->contentLength('2GB');
        $this->assertEquals('2147483648', $object->getHeaderLine('Content-Length'));

        $this->assertNotSame($object, $this->object);
    }

    public function testContentLength300Status(): void {
        $object = $this->object->withStatus(302)->contentLength(1234);
        $this->assertEquals('', $object->getHeaderLine('Content-Length'));

        $this->assertNotSame($object, $this->object);
    }

    public function testContentMD5(): void {
        $object = $this->object->contentMD5(md5('titon'));
        $this->assertEquals('940a8df7d359963b805f92e125dabecf', $object->getHeaderLine('Content-MD5'));

        $this->assertNotSame($object, $this->object);
    }

    public function testContentRange(): void {
        $object = $this->object->contentRange(0, 50, 100);
        $this->assertEquals('bytes 0-50/100', $object->getHeaderLine('Content-Range'));

        $this->assertNotSame($object, $this->object);
    }

    public function testContentType(): void {
        $object = $this->object->contentType('html');
        $this->assertEquals('text/html; charset=UTF-8', $object->getHeaderLine('Content-Type'));

        $object = $this->object->contentType('application/json');
        $this->assertEquals('application/json; charset=UTF-8', $object->getHeaderLine('Content-Type'));

        $object = $this->object->contentType('xhtml');
        $this->assertEquals('application/xhtml+xml', $object->getHeaderLine('Content-Type'));

        $this->assertNotSame($object, $this->object);
    }

    public function testContentType300Status(): void {
        $object = $this->object->withStatus(304)->contentType('xhtml');
        $this->assertEquals('', $object->getHeaderLine('Content-Type')); // Should not set

        $this->assertNotSame($object, $this->object);
    }

    /**
     * @expectedException \Titon\Utility\Exception\InvalidExtensionException
     */
    public function testContentTypeInvalidType(): void {
        $this->object->contentType('fake');
    }

    public function testDate(): void {
        $object = $this->object->date('+12 hours');
        $this->assertEquals(Format::http('+12 hours'), $object->getHeaderLine('Date'));

        $this->assertNotSame($object, $this->object);
    }

    public function testEtag(): void {
        $object = $this->object->etag('FooBar');
        $this->assertEquals('"FooBar"', $object->getHeaderLine('ETag'));

        $object = $this->object->etag('FooBar', true);
        $this->assertEquals('W/"FooBar"', $object->getHeaderLine('ETag'));

        $this->assertNotSame($object, $this->object);
    }

    public function testExpires(): void {
        $object = $this->object->expires('+12 hours');
        $this->assertEquals(Format::http('+12 hours'), $object->getHeaderLine('Expires'));

        $this->assertNotSame($object, $this->object);
    }

    public function testLastModified(): void {
        $object = $this->object->lastModified('+12 hours');
        $this->assertEquals(Format::http('+12 hours'), $object->getHeaderLine('Last-Modified'));

        $this->assertNotSame($object, $this->object);
    }

    public function testLocation(): void {
        $object = $this->object->location('/local/url');
        $this->assertEquals('/local/url', $object->getHeaderLine('Location'));

        $object = $this->object->location('http://google.com');
        $this->assertEquals('http://google.com', $object->getHeaderLine('Location'));

        $this->assertNotSame($object, $this->object);
    }

    public function testNoCache(): void {
        $object = $this->object->noCache();
        $this->assertEquals(Format::http('-1 year'), $object->getHeaderLine('Expires'));
        $this->assertEquals(Format::http(time()), $object->getHeaderLine('Last-Modified'));
        $this->assertEquals('no-cache, no-store, must-revalidate, proxy-revalidate', $object->getHeaderLine('Cache-Control'));

        $this->assertNotSame($object, $this->object);
    }

    public function testNotModified(): void {
        $object = $this->object->contentType('html')->notModified();
        $this->assertEquals('', $object->getHeaderLine('Content-Type')); // Removes it
        $this->assertEquals(304, $object->getStatusCode());

        $this->assertNotSame($object, $this->object);
    }

    public function testRemoveCookie(): void {
        $time = time();
        $object = $this->object->removeCookie('foo');

        $this->assertEquals('foo=deleted; Expires=' . gmdate(Http::DATE_FORMAT, $time) . '; Path=/; HttpOnly', $object->getHeaderLine('Set-Cookie'));

        $this->assertNotSame($object, $this->object);
    }

    public function testRetryAfter(): void {
        $object = $this->object->retryAfter(120);
        $this->assertEquals('120', $object->getHeaderLine('Retry-After'));

        $object = $this->object->retryAfter('+1 hour');
        $this->assertEquals(Format::http('+1 hour'), $object->getHeaderLine('Retry-After'));

        $this->assertNotSame($object, $this->object);
    }

    public function testSetCookie(): void {
        $time = strtotime('+1 week');
        $object = $this->object->setCookie('foo', 'bar', null, $time);

        $this->assertEquals('foo=bar; Expires=' . gmdate(Http::DATE_FORMAT, $time) . '; Path=/; HttpOnly', $object->getHeaderLine('Set-Cookie'));

        $this->assertNotSame($object, $this->object);
    }

    public function testVary(): void {
        $object = $this->object->vary('Accept');
        $this->assertEquals('Accept', $object->getHeaderLine('Vary'));

        $object = $this->object->vary('Accept, Cookie');
        $this->assertEquals('Accept, Cookie', $object->getHeaderLine('Vary'));

        $this->assertNotSame($object, $this->object);
    }

    public function testWwwAuthenticate(): void {
        $object = $this->object->wwwAuthenticate('Basic');
        $this->assertEquals('Basic', $object->getHeaderLine('WWW-Authenticate'));

        $object = $this->object->wwwAuthenticate('Digest');
        $this->assertEquals('Digest', $object->getHeaderLine('WWW-Authenticate'));

        $this->assertNotSame($object, $this->object);
    }

}
