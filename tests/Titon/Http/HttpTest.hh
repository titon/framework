<?hh
namespace Titon\Http;

use Titon\Test\TestCase;

class HttpTest extends TestCase {

    public function testGetHeaderTypes(): void {
        $this->assertEquals(Vector {
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
        $this->assertEquals(Vector {
            'GET',
            'POST',
            'PUT',
            'DELETE',
            'HEAD',
            'TRACE',
            'OPTIONS',
            'CONNECT'
        }, Http::getMethodTypes());
    }

    public function testGetStatusCodes(): void {
        $this->assertEquals(Map {
            100 => 'Continue',
            101 => 'Switching Protocols',
            102 => 'Processing',
            200 => 'OK',
            201 => 'Created',
            202 => 'Accepted',
            203 => 'Non-Authoritative Information',
            204 => 'No Content',
            205 => 'Reset Content',
            206 => 'Partial Content',
            207 => 'Multi-Status',
            300 => 'Multiple Choices',
            301 => 'Moved Permanently',
            302 => 'Found',
            303 => 'See Other',
            304 => 'Not Modified',
            305 => 'Use Proxy',
            307 => 'Temporary Redirect',
            400 => 'Bad Request',
            401 => 'Unauthorized',
            402 => 'Payment Required',
            403 => 'Forbidden',
            404 => 'Not Found',
            405 => 'Method Not Allowed',
            406 => 'Not Acceptable',
            407 => 'Proxy Authentication Required',
            408 => 'Request Timeout',
            409 => 'Conflict',
            410 => 'Gone',
            411 => 'Length Required',
            412 => 'Precondition Failed',
            413 => 'Request Entity Too Large',
            414 => 'Request-URI Too Long',
            415 => 'Unsupported Media Type',
            416 => 'Requested Range Not Satisfiable',
            417 => 'Expectation Failed',
            422 => 'Unprocessable Entity',
            423 => 'Locked',
            424 => 'Failed Dependency',
            428 => 'Precondition Required',
            429 => 'Too Many Requests',
            431 => 'Request Header Fields Too Large',
            500 => 'Internal Server Error',
            501 => 'Not Implemented',
            502 => 'Bad Gateway',
            503 => 'Service Unavailable',
            504 => 'Gateway Timeout',
            505 => 'HTTP Version Not Supported',
            507 => 'Insufficient Storage',
            508 => 'Loop Detected',
            510 => 'Not Extended',
            511 => 'Network Authentication Required',
        }, Http::getStatusCodes());
    }

    public function testGetStatusCode(): void {
        $this->assertEquals('OK', Http::getStatusCode(200));
    }

    /**
     * @expectedException \Titon\Http\Exception\InvalidStatusException
     */
    public function testGetStatusCodeInvalidCode(): void {
        Http::getStatusCode(999);
    }

}
