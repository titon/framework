<?hh
namespace Titon\Http;

use Titon\Test\TestCase;

class HttpTest extends TestCase {

    public function testGetHeaderTypes(): void {
        $this->assertEquals(Set {
            'Accept',
            'Accept-Charset',
            'Accept-Encoding',
            'Accept-Language',
            'Accept-Ranges',
            'Access-Control-Allow-Origin',
            'Age',
            'Allow',
            'Authentication-Info',
            'Authorization',
            'Cache-Control',
            'Connection',
            'Content-Disposition',
            'Content-Encoding',
            'Content-Language',
            'Content-Length',
            'Content-Location',
            'Content-MD5',
            'Content-Range',
            'Content-Type',
            'Cookie',
            'Date',
            'ETag',
            'Expires',
            'Expect',
            'From',
            'Host',
            'If-Match',
            'If-Modified-Since',
            'If-None-Match',
            'If-Range',
            'If-Unmodified-Since',
            'Keep-Alive',
            'Last-Modified',
            'Link',
            'Location',
            'Max-Forwards',
            'Origin',
            'P3P',
            'Pragma',
            'Proxy-Authenticate',
            'Proxy-Authorization',
            'Range',
            'Referer',
            'Refresh',
            'Retry-After',
            'Server',
            'Set-Cookie',
            'Status',
            'Strict-Transport-Security',
            'TE',
            'Trailer',
            'Transfer-Encoding',
            'Upgrade',
            'User-Agent',
            'Vary',
            'Via',
            'Warning',
            'WWW-Authenticate'
        }, Http::getHeaderTypes());
    }

    public function testGetMethodTypes(): void {
        $this->assertEquals(Set {
            'GET',
            'POST',
            'PUT',
            'DELETE',
            'HEAD',
            'TRACE',
            'OPTIONS',
            'CONNECT',
            'PATCH'
        }, Http::getMethodTypes());
    }

}
