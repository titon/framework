<?hh
namespace Titon\Crypto;

use Titon\Test\TestCase;

class McryptCipherTest extends TestCase {

    /**
     * @dataProvider getCipherMethods
     */
    public function testEncryptDecrypt($method): void {
        $cipher = new McryptCipher(md5('RandOmStringKeyHere'), $method);
        $string = 'This string will be used to encrypt and decrypt. It contains numbers: 1 2 3 4 5 6 7 8 9 0. It also contains punctuation: ! @ # $ % ^ & * ( ) : [ < ?. You get the picture.';

        $encrypted = $cipher->encrypt($string);
        $decrypted = $cipher->decrypt($encrypted);

        $this->assertEquals($string, $decrypted);
    }

    public function getCipherMethods(): array<array<string>> {
        return array_map(function($value) {
            return [$value];
        }, McryptCipher::getSupportedMethods());
    }

    public function testIsSupportedMethod(): void {
        $this->assertTrue(McryptCipher::isSupportedMethod('AES-256-CBC'));
        $this->assertFalse(McryptCipher::isSupportedMethod('foo'));
    }

    /**
     * @expectedException \Titon\Crypto\Exception\UnsupportedCipherException
     */
    public function testIsSupportedMethodThrowsException(): void {
        $cipher = new McryptCipher('RandOmStringKeyHere', 'foo');
    }

    public function testIsValidKey(): void {
        $this->assertTrue(McryptCipher::isValidKey('ThisIsA32CharacterStringWooooooh'));
        $this->assertFalse(McryptCipher::isValidKey('ThisIsnt'));
    }

    /**
     * @expectedException \Titon\Crypto\Exception\InvalidKeyException
     */
    public function testIsValidKeyThrowsException(): void {
        $cipher = new McryptCipher('foo', 'AES-256-CBC');
    }

}
