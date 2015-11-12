<?hh
namespace Titon\Http;

use Titon\Crypto\OpenSslCipher;
use Titon\Test\TestCase;

class CookieTest extends TestCase {

    public function testSetupState(): void {
        $cookie = new Cookie('foo', 'bar', '+10 minutes', '/baz', 'domain.com');

        $this->assertEquals('foo=bar; Expires=' . gmdate('D, d M Y H:i:s T', $cookie->getExpires()) . '; Path=/baz; Domain=domain.com; HttpOnly', (string) $cookie);
    }

    public function testValueCanBeEncryptedAndDecrypted(): void {
        $cipher = new OpenSslCipher(md5('CookieTestSalt'));

        $cookie = new Cookie('foo');
        $cookie->setValue('bar', $cipher);

        $this->assertNotEquals('bar', $cookie->getValue());
        $this->assertEquals('bar', $cookie->getValue($cipher));
    }

    public function testFlagsAlterOutputString(): void {
        $cookie = new Cookie('foo', 'bar');
        $cookie->setExpires('-10 minutes');
        $cookie->setSecure(true);

        $this->assertEquals('foo=deleted; Expires=' . gmdate('D, d M Y H:i:s T', $cookie->getExpires()) . '; Path=/; Secure; HttpOnly', (string) $cookie);
    }

    /**
     * @expectedException \InvalidArgumentException
     */
    public function testNameFailsOnInvalidCharacters(): void {
        new Cookie('foo;bar', '');
    }

}
