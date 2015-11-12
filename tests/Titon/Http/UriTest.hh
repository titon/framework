<?hh
namespace Titon\Http;

use Titon\Test\TestCase;

/**
 * @property \Titon\Http\Uri $object
 */
class UriTest extends TestCase {

    protected function setUp(): void {
        parent::setUp();

        $this->object = new Uri();
    }

    public function testConstructorParses(): void {
        $object = new Uri('https://user:pass@sub.domain.com:1337/foo?bar=baz#quz');

        $this->assertEquals('https', $object->getScheme());
        $this->assertEquals('user:pass', $object->getUserInfo());
        $this->assertEquals('sub.domain.com', $object->getHost());
        $this->assertEquals(1337, $object->getPort());
        $this->assertEquals('user:pass@sub.domain.com:1337', $object->getAuthority());
        $this->assertEquals('/foo', $object->getPath());
        $this->assertEquals('bar=baz', $object->getQuery());
        $this->assertEquals('quz', $object->getFragment());
    }

    public function testToString(): void {
        $url = 'https://user:pass@local.example.com:3001/foo?bar=baz#quz';
        $object = new Uri($url);

        $this->assertEquals($url, (string) $object);
    }

    public function testEncode(): void {
        $this->assertSame('', $this->object->encode(''));

        $this->assertSame('fo%5Eo', $this->object->encode('fo^o'));

        $this->assertSame('fo%5Eo', $this->object->encode('fo%5Eo')); // No double encode
    }

    public function testFilterFragment(): void {
        $object = $this->object->withFragment('');

        $this->assertSame('', $object->getFragment());

        $object = $this->object->withFragment('#foo');

        $this->assertSame('foo', $object->getFragment());

        $object = $this->object->withFragment('#foo=bar');

        $this->assertSame('foo%3Dbar', $object->getFragment());

        $object = $this->object->withFragment('#foo%3Dbar');

        $this->assertSame('foo%3Dbar', $object->getFragment());
    }

    public function testFilterHost(): void {
        $object = $this->object->withHost('localhost');

        $this->assertSame('localhost', $object->getHost());

        $object = $this->object->withHost('TitonFramework.com');

        $this->assertSame('titonframework.com', $object->getHost());
    }

    /**
     * @expectedException \InvalidArgumentException
     */
    public function testFilterHostErrorsStartingHyphen(): void {
        $this->object->withHost('-localhost');
    }

    /**
     * @expectedException \InvalidArgumentException
     */
    public function testFilterHostErrorsEndingHyphen(): void {
        $this->object->withHost('localhost-');
    }

    /**
     * @expectedException \InvalidArgumentException
     */
    public function testFilterHostErrorsInvalidChars(): void {
        $this->object->withHost('local%=host');
    }

    public function testFilterUserPassword(): void {
        $object = $this->object->withUserInfo('user', 'password');

        $this->assertSame('user:password', $object->getUserInfo());

        $object = $this->object->withUserInfo('us~er', 'sj738#*$jsa');

        $this->assertSame('us~er:sj738%23%2A%24jsa', $object->getUserInfo());
    }

    public function testFilterPath(): void {
        $object = $this->object->withPath('');

        $this->assertSame('', $object->getPath());

        $object = $this->object->withPath('/');

        $this->assertSame('/', $object->getPath());

        $object = $this->object->withPath('/foo');

        $this->assertSame('/foo', $object->getPath());

        $object = $this->object->withPath('foo');

        $this->assertSame('foo', $object->getPath());

        $object = $this->object->withPath('/foo/bar-baz/1/2');

        $this->assertSame('/foo/bar-baz/1/2', $object->getPath());
    }

    /**
     * @expectedException \InvalidArgumentException
     */
    public function testFilterPathErrorsQueryString(): void {
        $this->object->withPath('/foo?bar=baz');
    }

    /**
     * @expectedException \InvalidArgumentException
     */
    public function testFilterPathErrorsFragment(): void {
        $this->object->withPath('/foo#bar');
    }

    public function testFilterPort(): void {
        $object = $this->object->withPort('123');

        $this->assertSame(123, $object->getPort());

        $object = $this->object->withPort(32152);

        $this->assertSame(32152, $object->getPort());
    }

    /**
     * @expectedException \InvalidArgumentException
     */
    public function testFilterPortErrorsInvalidPort(): void {
        $this->object->withPort(-123);
    }

    public function testFilterQuery(): void {
        $object = $this->object->withQuery('');

        $this->assertSame('', $object->getQuery());

        $object = $this->object->withQuery('foo=bar');

        $this->assertSame('foo=bar', $object->getQuery());

        $object = $this->object->withQuery('?foo=bar&bar=123');

        $this->assertSame('foo=bar&bar=123', $object->getQuery());

        $object = $this->object->withQuery('?foo=bar&bar=123&baz');

        $this->assertSame('foo=bar&bar=123&baz', $object->getQuery());

        $object = $this->object->withQuery('fo^o=ba`r');

        $this->assertSame('fo%5Eo=ba%60r', $object->getQuery());

        $object = $this->object->withQuery('k^ey&key[]=valu`&f<>=`bar');

        $this->assertSame('k%5Eey&key%5B%5D=valu%60&f%3C%3E=%60bar', $object->getQuery());
    }

    public function testFilterScheme(): void {
        $object = $this->object->withScheme('');

        $this->assertSame('', $object->getScheme());

        $object = $this->object->withScheme('http:');

        $this->assertSame('http', $object->getScheme());

        $object = $this->object->withScheme('HtTp://');

        $this->assertSame('http', $object->getScheme());
    }

    /**
     * @expectedException \InvalidArgumentException
     */
    public function testFilterSchemeErrorsStartsNumber(): void {
         $this->object->withScheme('0ttp');
    }

    /**
     * @expectedException \InvalidArgumentException
     */
    public function testFilterSchemeErrorsInvalidChars(): void {
        $this->object->withScheme('h&%p');
    }

    public function testGetAuthority(): void {
        $object = $this->object->withHost('');

        $this->assertSame('', $object->getAuthority());

        $object = $object->withHost('titon.io');

        $this->assertSame('titon.io', $object->getAuthority());

        $object = $object->withUserInfo('user');

        $this->assertSame('user@titon.io', $object->getAuthority());

        $object = $object->withUserInfo('user', 'pass');

        $this->assertSame('user:pass@titon.io', $object->getAuthority());

        $object = $object->withPort(123);

        $this->assertSame('user:pass@titon.io:123', $object->getAuthority());
    }

    public function testGetPort(): void {
        $object = $this->object->withPort(null);

        $this->assertSame(null, $object->getPort());

        $object = $this->object->withPort(123);

        $this->assertSame(123, $object->getPort());
    }

    public function testGetPortIsStandard(): void {
        $object = $this->object->withPort(80);
        $object = $object->withScheme('http');

        $this->assertSame(null, $object->getPort());
    }

    public function testIsStandardPort(): void {
        $this->assertTrue($this->object->isStandardPort(80, 'http'));
        $this->assertTrue($this->object->isStandardPort(443, 'https'));
        $this->assertFalse($this->object->isStandardPort(8080, 'http'));
        $this->assertFalse($this->object->isStandardPort(22, 'ssh'));
    }

    public function testWithFragment(): void {
        $object1 = $this->object->withFragment('foo');
        $object2 = $object1->withFragment('bar');

        $this->assertNotSame($object1, $object2);
        $this->assertNotSame($object1->getFragment(), $object2->getFragment());
    }

    public function testWithHost(): void {
        $object1 = $this->object->withHost('titon.io');
        $object2 = $object1->withHost('titon.com');

        $this->assertNotSame($object1, $object2);
        $this->assertNotSame($object1->getHost(), $object2->getHost());
    }

    public function testWithPath(): void {
        $object1 = $this->object->withPath('/foo');
        $object2 = $object1->withPath('/bar');

        $this->assertNotSame($object1, $object2);
        $this->assertNotSame($object1->getPath(), $object2->getPath());
    }

    public function testWithPort(): void {
        $object1 = $this->object->withPort(123);
        $object2 = $object1->withPort(456);

        $this->assertNotSame($object1, $object2);
        $this->assertNotSame($object1->getPort(), $object2->getPort());
    }

    public function testWithQuery(): void {
        $object1 = $this->object->withQuery('foo=bar');
        $object2 = $object1->withQuery('foo=baz');

        $this->assertNotSame($object1, $object2);
        $this->assertNotSame($object1->getQuery(), $object2->getQuery());
    }

    public function testWithScheme(): void {
        $object1 = $this->object->withScheme('http');
        $object2 = $object1->withScheme('https');

        $this->assertNotSame($object1, $object2);
        $this->assertNotSame($object1->getScheme(), $object2->getScheme());
    }

    public function testWithUserInfo(): void {
        $object1 = $this->object->withUserInfo('user');
        $object2 = $object1->withUserInfo('user', 'pass');

        $this->assertNotSame($object1, $object2);
        $this->assertNotSame($object1->getUserInfo(), $object2->getUserInfo());
    }

}
