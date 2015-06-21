<?hh
namespace Titon\Crypto;

use Titon\Test\TestCase;

class OpenSslCipherTest extends TestCase {

    /**
     * @dataProvider getCipherMethods
     */
    public function testEncryptDecrypt($method): void {
        $cipher = new OpenSslCipher(md5('RandOmStringKeyHere'), $method);
        $string = 'This string will be used to encrypt and decrypt. It contains numbers: 1 2 3 4 5 6 7 8 9 0. It also contains punctuation: ! @ # $ % ^ & * ( ) : [ < ?. You get the picture.';

        $encrypted = $cipher->encrypt($string);
        $decrypted = $cipher->decrypt($encrypted);

        $this->assertEquals($string, $decrypted);
    }

    public function getCipherMethods(): array<array<string>> {
        return array_map(function($value) {
            return [$value];
        }, OpenSslCipher::getSupportedMethods());
    }

    public function testIsSupportedMethod(): void {
        $this->assertTrue(OpenSslCipher::isSupportedMethod('AES-128-CBC'));
        $this->assertFalse(OpenSslCipher::isSupportedMethod('foo'));
    }

    /**
     * @expectedException \Titon\Crypto\Exception\UnsupportedCipherException
     */
    public function testIsSupportedMethodThrowsException(): void {
        $cipher = new OpenSslCipher('RandOmStringKeyHere', 'foo');
    }

    public function testIsValidKey(): void {
        $this->assertTrue(OpenSslCipher::isValidKey('ThisIsA32CharacterStringWooooooh'));
        $this->assertFalse(OpenSslCipher::isValidKey('ThisIsnt'));
    }

    /**
     * @expectedException \Titon\Crypto\Exception\InvalidKeyException
     */
    public function testIsValidKeyThrowsException(): void {
        $cipher = new OpenSslCipher('foo', 'AES-128-CBC');
    }

}
