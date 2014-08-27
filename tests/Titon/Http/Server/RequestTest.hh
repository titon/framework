<?hh
namespace Titon\Http\Server;

use Titon\Test\TestCase;
use Titon\Utility\Crypt;

/**
 * @property \Titon\Http\Server\Request $object
 */
class RequestTest extends TestCase {

    protected function setUp() {
        parent::setUp();

        $_GET = [
            'key' => 'value',
            'Model' => [
                'foo' => 'bar'
            ]
        ];

        $_POST = [
            '_method' => 'PUT',
            'key' => 'value',
            'Model' => [
                'foo' => 'baz'
            ]
        ];

        $_COOKIE = [
            'foo' => Crypt::rijndael(base64_encode(serialize('bar')), 'foo')
        ];

        $_FILES = [
            'file' => [
                'name' => 'file1.jpg',
                'type' => 'image/jpeg',
                'tmp_name' => '/tmp/phpUkYTB5',
                'error' => 0,
                'size' => 307808
            ],
            'two' => [
                'name' => ['file' => 'file2.png'],
                'type' => ['file' => 'image/png'],
                'tmp_name' => ['file' => '/tmp/phpo3HxIF'],
                'error' => ['file' => 0],
                'size' => ['file' => 10554]
            ],
            'three' => [
                'name' => [
                    'two' => ['file' => 'file3.png'],
                ],
                'type' => [
                    'two' => ['file' => 'image/png'],
                ],
                'tmp_name' => [
                    'two' => ['file' => '/tmp/phpgUtcPf'],
                ],
                'error' => [
                    'two' => ['file' => 0],
                ],
                'size' => [
                    'two' => ['file' => 19571],
                ]
            ],
            'four' => [
                'name' => [
                    'three' => [
                        'two' => ['file' => 'file4.jpg'],
                    ],
                ],
                'type' => [
                    'three' => [
                        'two' => ['file' => 'image/jpeg'],
                    ],
                ],
                'tmp_name' => [
                    'three' => [
                        'two' => ['file' => '/tmp/phpMTxSVP'],
                    ],
                ],
                'error' => [
                    'three' => [
                        'two' => ['file' => 0],
                    ],
                ],
                'size' => [
                    'three' => [
                        'two' => ['file' => 307808],
                    ],
                ],
            ],
        ];

        $this->object = Request::createFromGlobals();
    }

    public function testInitialize() {
        $this->assertEquals(Map {
            'key' => 'value',
            'Model' => Map {
                'foo' => 'bar'
            }
        }, $this->object->get->all());

        $this->assertEquals(Map {
            'key' => 'value',
            'Model' => Map {
                'foo' => 'baz'
            }
        }, $this->object->post->all());

        $this->assertEquals(Map {
            'file' => Map {
                'name' => 'file1.jpg',
                'type' => 'image/jpeg',
                'tmp_name' => '/tmp/phpUkYTB5',
                'error' => 0,
                'size' => 307808
            },
            'two' => Map {
                'file' => Map {
                    'name' => 'file2.png',
                    'type' => 'image/png',
                    'tmp_name' => '/tmp/phpo3HxIF',
                    'error' => 0,
                    'size' => 10554
                },
            },
            'three' => Map {
                'two' => Map {
                    'file' => Map {
                        'name' => 'file3.png',
                        'type' => 'image/png',
                        'tmp_name' => '/tmp/phpgUtcPf',
                        'error' => 0,
                        'size' => 19571
                    },
                },
            },
            'four' => Map {
                'three' => Map {
                    'two' => Map {
                        'file' => Map {
                            'name' => 'file4.jpg',
                            'type' => 'image/jpeg',
                            'tmp_name' => '/tmp/phpMTxSVP',
                            'error' => 0,
                            'size' => 307808
                        },
                    },
                },
            },
        }, $this->object->files->all());
    }

    public function testClone() {
        $clone = clone $this->object;

        $this->assertEquals($clone->internal, $this->object->internal);
        $this->assertEquals($clone->headers, $this->object->headers);
        $this->assertEquals($clone->get, $this->object->get);
        $this->assertEquals($clone->post, $this->object->post);
        $this->assertEquals($clone->files, $this->object->files);
        $this->assertEquals($clone->cookies, $this->object->cookies);
        $this->assertEquals($clone->server, $this->object->server);
    }

    public function testAccepts() {
        $this->object->headers->set('Accept', 'text/xml,application/xml;q=0.9,application/xhtml+xml,text/html,text/plain,image/png');

        $this->assertEquals(Map {'type' =>'text/html', 'quality' => 1}, $this->object->accepts('html'));
        $this->assertEquals(Map {'type' =>'application/xhtml+xml', 'quality' => 1}, $this->object->accepts('xhtml'));
        $this->assertEquals(Map {'type' =>'application/xml', 'quality' => 0.9}, $this->object->accepts('xml'));
        $this->assertEquals(null, $this->object->accepts('json'));

        $this->object->headers->set('Accept', 'application/json,*/*');

        $this->assertEquals(Map {'type' =>'application/json', 'quality' => 1}, $this->object->accepts('json'));
        $this->assertEquals(Map {'type' =>'*/*', 'quality' => 1}, $this->object->accepts('html')); // */*

        $this->object->headers->set('Accept', 'text/*');

        $this->assertEquals(Map {'type' =>'text/*', 'quality' => 1}, $this->object->accepts('text/html'));
        $this->assertEquals(Map {'type' =>'text/*', 'quality' => 1}, $this->object->accepts(['text/xml', 'application/json']));
        $this->assertEquals(null, $this->object->accepts('application/json'));
    }

    public function testAcceptsCharset() {
        $this->object->headers->set('Accept-Charset', 'UTF-8');

        $this->assertEquals(Map {'type' =>'utf-8', 'quality' => 1}, $this->object->acceptsCharset('utf-8'));
        $this->assertEquals(null, $this->object->acceptsCharset('iso-8859-1'));

        $this->object->headers->set('Accept-Charset', 'ISO-8859-1');

        $this->assertEquals(Map {'type' =>'iso-8859-1', 'quality' => 1}, $this->object->acceptsCharset('iso-8859-1'));
    }

    public function testAcceptsEncoding() {
        $this->object->headers->set('Accept-Encoding', 'compress;q=0.5, gzip;q=1.0');

        $this->assertEquals(Map {'type' =>'gzip', 'quality' => 1}, $this->object->acceptsEncoding('gzip'));
        $this->assertEquals(null, $this->object->acceptsEncoding('identity'));
    }

    public function testAcceptsLanguage() {
        $this->object->headers->set('Accept-Language', 'en-us,en;q=0.8,fr-fr;q=0.5,fr;q=0.3');

        $this->assertEquals(Map {'type' =>'en-us', 'quality' => 1}, $this->object->acceptsLanguage('en-US'));
        $this->assertEquals(Map {'type' =>'en', 'quality' => 0.8}, $this->object->acceptsLanguage('en'));
        $this->assertEquals(Map {'type' =>'fr', 'quality' => 0.3}, $this->object->acceptsLanguage('fr'));
        $this->assertEquals(null, $this->object->acceptsLanguage('DE'));
    }

    public function testGetClientIp() {
        $this->object->server->set('REMOTE_ADDR', '192.168.1.2');
        $this->object->server->set('HTTP_CLIENT_IP', '192.168.1.1');

        $this->assertEquals('192.168.1.2', $this->object->getClientIP());

        $this->object->server->remove('REMOTE_ADDR');

        $this->assertEquals('192.168.1.1', $this->object->getClientIP());

        $this->object->server->set('HTTP_X_FORWARDED_FOR', '192.168.1.3, 10.0.255.255, proxy.10.1');

        $this->assertEquals('192.168.1.3', $this->object->getClientIP());

        $this->object->setConfig('trustProxies', false);

        $this->assertEquals('192.168.1.1', $this->object->getClientIP());
    }

    public function testGetCookie() {
        $this->assertEquals(null, $this->object->getCookie('key'));
        $this->assertEquals('bar', $this->object->getCookie('foo')); // decrypted
    }

    public function testGetCookies() {
        $this->assertEquals(Map {
            'foo' => 'bar'
        }, $this->object->getCookies());
    }

    public function testGetHost() {
        $this->object->server->set('HTTP_HOST', 'titon.io');
        $this->assertEquals('titon.io', $this->object->getHost());

        $this->object->server->set('HTTP_HOST', 'titon.io:8080');
        $this->assertEquals('titon.io', $this->object->getHost());

        $this->object->server->remove('HTTP_HOST');
        $this->object->server->set('SERVER_NAME', 'titon-s1');
        $this->assertEquals('titon-s1', $this->object->getHost());

        $this->object->server->remove('SERVER_NAME');
        $this->object->server->set('SERVER_ADDR', '74.232.443.122');
        $this->assertEquals('74.232.443.122', $this->object->getHost());

        $this->object->server->set('HTTP_X_FORWARDED_HOST', 'titon-proxy.com');
        $this->assertEquals('titon-proxy.com', $this->object->getHost());

        $this->object->setConfig('trustProxies', false);
        $this->assertEquals('74.232.443.122', $this->object->getHost());
    }

    public function testGetMethod() {
        $this->assertEquals('PUT', $this->object->getMethod()); // set in $_POST
    }

    public function testGetMethodOverride() {
        $_SERVER['REQUEST_METHOD'] = 'POST';
        $_SERVER['HTTP_X_METHOD_OVERRIDE'] = 'DELETE';

        // New instance so it won't inherit from setup
        $request = Request::createFromGlobals();

        $this->assertEquals('DELETE', $request->getMethod());
    }

    public function testGetProtocolVersion() {
        $this->assertEquals('1.1', $this->object->getProtocolVersion());

        $this->object->server->set('SERVER_PROTOCOL', '1.0');
        $this->assertEquals('1.0', $this->object->getProtocolVersion());
    }

    public function testGetPort() {
        $this->object->server->set('HTTP_HOST', 'titon.io');
        $this->assertEquals(80, $this->object->getPort());

        $this->object->server->set('HTTP_HOST', 'titon.io:8080');
        $this->assertEquals(8080, $this->object->getPort());

        $this->object->server->set('HTTP_HOST', 'titon.io');
        $this->object->server->set('HTTPS', 'on');
        $this->assertEquals(443, $this->object->getPort());

        $this->object->server->set('SERVER_PORT', 88);
        $this->object->server->remove('HTTP_HOST');
        $this->assertEquals(88, $this->object->getPort());

        $this->object->server->add(Map {
            'HTTP_X_FORWARDED_PORT' => 77,
            'HTTP_X_FORWARDED_PROTO' => 'https'
        });

        $this->assertEquals(77, $this->object->getPort());

        $this->object->setConfig('trustProxies', false);

        $this->assertEquals(88, $this->object->getPort());

        $this->object->setConfig('trustProxies', true);

        $this->object->server->remove('HTTP_X_FORWARDED_PORT');

        $this->assertEquals(443, $this->object->getPort());

        $this->object->server->remove('HTTP_X_FORWARDED_PROTO');

        $this->assertEquals(88, $this->object->getPort());
    }

    public function testGetReferrer() {
        $this->object->server->set('HTTP_REFERER', '');
        $this->assertEquals('/', $this->object->getReferrer());

        $this->object->server->set('HTTP_REFERER', 'http://google.com');
        $this->assertEquals('http://google.com', $this->object->getReferrer());

        $this->object->server->set('HTTP_REFERER', 'http://localhost/module/controller');
        $this->assertEquals('/module/controller', $this->object->getReferrer());
    }

    public function testGetScheme() {
        $this->assertEquals('http', $this->object->getScheme());

        $this->object->server->set('HTTPS', 'on');
        $this->assertEquals('https', $this->object->getScheme());
    }

    public function testGetServerIp() {
        $this->object->server->set('SERVER_ADDR', '127.0.0.1');
        $this->assertEquals('127.0.0.1', $this->object->getServerIP());
    }

    public function testGetUrl() {
        $this->object->server->add(Map {
            'SCRIPT_FILENAME' => '/var/www/titon/app/web/index.php',
            'DOCUMENT_ROOT' => '/var/www/titon/app/web',
            'PATH_INFO' => '/en/some/path/',
            'REQUEST_URI' => '/en/some/path/?key=value', // invalid
            'PHP_SELF' => '/index.php/en/some/path/'
        });

        $this->assertEquals('/en/some/path/', $this->object->getUrl()); // PATH_INFO

        $this->object->server->remove('PATH_INFO');
        $this->object->setUrl(null);

        $this->assertEquals('/en/some/path/', $this->object->getUrl()); // REQUEST_URI without query

        $this->object->server->remove('REQUEST_URI');
        $this->object->setUrl(null);

        $this->assertEquals('/en/some/path/', $this->object->getUrl()); // PHP_SELF
    }

    public function testGetUrlBaseFolder() {
        $this->object->server->add(Map {
            'SCRIPT_FILENAME' => '/var/www/titon/app/web/root/index.php',
            'DOCUMENT_ROOT' => '/var/www/titon/app/web',
            'PATH_INFO' => '/en/some/path/',
            'REQUEST_URI' => '/en/some/path/',
            'PHP_SELF' => '/root/index.php/en/some/path/'
        });

        $this->assertEquals('/root/en/some/path/', $this->object->getUrl()); // PATH_INFO

        $this->object->server->remove('PATH_INFO');
        $this->object->setUrl(null);

        $this->assertEquals('/root/en/some/path/', $this->object->getUrl()); // REQUEST_URI without query

        $this->object->server->remove('REQUEST_URI');
        $this->object->setUrl(null);

        $this->assertEquals('/root/en/some/path/', $this->object->getUrl()); // PHP_SELF
    }

    public function testGetUserAgent() {
        $this->object->server->set('HTTP_USER_AGENT', 'Mozilla/5.0 (X11; Linux x86_64; rv:12.0) Gecko/20100101 Firefox/12.0');
        $this->assertEquals('Mozilla/5.0 (X11; Linux x86_64; rv:12.0) Gecko/20100101 Firefox/12.0', $this->object->getUserAgent());
    }

    public function testIsAjax() {
        $this->object->server->set('HTTP_X_REQUESTED_WITH', 'XMLHttpRequest');
        $this->assertTrue($this->object->isAJAX());

        $this->object->server->remove('HTTP_X_REQUESTED_WITH');
        $this->assertFalse($this->object->isAJAX());
    }

    public function testIsMethods() {
        $this->assertTrue($this->object->isPut()); // set in $_POST

        $this->object->setMethod('get');
        $this->assertTrue($this->object->isGet());

        $this->object->setMethod('DELETE');
        $this->assertTrue($this->object->isDelete());

        $this->object->setMethod('post');
        $this->assertTrue($this->object->isPost());

        $this->object->setMethod('PUT');
        $this->assertTrue($this->object->isMethod('put'));
    }

    public function testIsFlash() {
        $this->object->server->set('HTTP_USER_AGENT', '');
        $this->assertFalse($this->object->isFlash());

        $this->object->server->set('HTTP_USER_AGENT', 'Shockwave Flash');
        $this->assertTrue($this->object->isFlash());

        $this->object->server->set('HTTP_USER_AGENT', 'Adobe Flash');
        $this->assertTrue($this->object->isFlash());
    }

    public function testIsMobile() {
        $this->object->server->set('HTTP_USER_AGENT', '');
        $this->assertFalse($this->object->isMobile());

        $this->object->server->set('HTTP_USER_AGENT', 'Mozilla/5.0 (Linux; U; Android 4.0.3; ko-kr; LG-L160L Build/IML74K) AppleWebkit/534.30 (KHTML, like Gecko) Version/4.0 Mobile Safari/534.30');
        $this->assertTrue($this->object->isMobile());

        $this->object->server->set('HTTP_USER_AGENT', 'Mozilla/5.0 (BlackBerry; U; BlackBerry 9800; it) AppleWebKit/534.8+ (KHTML, like Gecko) Version/6.0.0.668 Mobile Safari/534.8+');
        $this->assertTrue($this->object->isMobile());

        $this->object->server->set('HTTP_USER_AGENT', 'Nokia5320XpressMusic/GoBrowser/1.6.0.70');
        $this->assertTrue($this->object->isMobile());

        $this->object->server->set('HTTP_USER_AGENT', 'Opera/9.80 (J2ME/MIDP; Opera Mini/5.0.18741/870; U; fr) Presto/2.4.15');
        $this->assertTrue($this->object->isMobile());

        $this->object->server->set('HTTP_USER_AGENT', 'iPhone');
        $this->assertTrue($this->object->isMobile());
    }

    public function testIsSecure() {
        $this->object->server->set('HTTPS', 'off');
        $this->object->server->set('SERVER_PORT', 80);

        $this->assertFalse($this->object->isSecure());

        $this->object->server->set('HTTPS', 'on');
        $this->object->server->set('SERVER_PORT', 80);

        $this->assertTrue($this->object->isSecure());

        $this->object->server->set('HTTPS', 'off');
        $this->object->server->set('SERVER_PORT', 443);

        $this->assertTrue($this->object->isSecure());

        $this->object->server->set('HTTPS', 'on');
        $this->object->server->remove('SERVER_PORT');

        $this->assertTrue($this->object->isSecure());

        $this->object->server->set('HTTPS', 0);

        $this->assertFalse($this->object->isSecure());

        $this->object->server->set('HTTP_X_FORWARDED_PROTO', 'https');

        $this->assertTrue($this->object->isSecure());
    }

    public function testSetMethod() {
        $this->assertEquals('PUT', $this->object->getMethod());

        $this->object->setMethod('delete');
        $this->assertEquals('DELETE', $this->object->getMethod());
    }

    /**
     * @expectedException \Titon\Http\Exception\InvalidMethodException
     */
    public function testSetMethodInvalidMethod() {
        $this->object->setMethod('FOO');
    }

}