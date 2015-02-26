<?hh
namespace Titon\Http;

use Titon\Test\TestCase;

class CookieTest extends TestCase {

    public function testSetupState(): void {
        $cookie = new Cookie('foo', 'bar', '+10 minutes', '/baz', 'domain.com');

        $this->assertEquals('foo=5hxAThObwiiTyh0mhfxIKw%3D%3D; Expires=' . gmdate('D, d M Y H:i:s T', strtotime('+10 minutes')) . '; Path=/baz; Domain=domain.com; HttpOnly', (string) $cookie);
    }

    public function testValueCanBeEncryptedAndDecrypted(): void {
        $cookie = new Cookie('foo', 'bar');

        $this->assertEquals('bar', $cookie->getValue());
        $this->assertEquals('5hxAThObwiiTyh0mhfxIKw%3D%3D', $cookie->getEncryptedValue());

        $cookie = new Cookie('foo', '5hxAThObwiiTyh0mhfxIKw%3D%3D');

        $this->assertEquals('5hxAThObwiiTyh0mhfxIKw%3D%3D', $cookie->getValue());
        $this->assertEquals('bar', $cookie->getDecryptedValue());
    }

    public function testFlagsAlterOutputString(): void {
        $cookie = new Cookie('foo', 'bar');
        $cookie->setExpires('-10 minutes');
        $cookie->setSecure(true);

        $this->assertEquals('foo=deleted; Expires=' . gmdate('D, d M Y H:i:s T', strtotime('-10 minutes')) . '; Path=/; Secure; HttpOnly', (string) $cookie);
    }

    /**
     * @expectedException \InvalidArgumentException
     */
    public function testNameFailsOnInvalidCharacters(): void {
        new Cookie('foo;bar', '');
    }

}
