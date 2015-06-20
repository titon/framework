<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Utility;

/**
 * Crypt provides methods for encrypting and decrypting data using popular algorithms like Rijndael, Blowfish and DES.
 * Also provides convenience methods for basic hashing and obfuscation that utilizes configuration settings like salts.
 *
 * @package Titon\Utility
 */
class Crypt {

    const string SALT = 'dDFUMG4gZlI0bTNXMFJr';

    /**
     * Check if a cipher method is supported.
     *
     * @param string $cipher
     * @return string
     */
    public static function checkCipher(string $cipher): string {
        if (!in_array($cipher, openssl_get_cipher_methods())) {
            throw new UnsupportedCipherException(sprintf('Unsupported cipher [%s]', $cipher));
        }

        return $cipher;
    }

    /**
     * Generate an IV to use for encryption and decryption.
     *
     * @return string
     */
    public static function createIV(): string {
        return openssl_random_pseudo_bytes(16);
    }

    /**
     * Decrypt an encrypted string using the passed cipher and mode.
     * Additional types of ciphers and modes can be used that aren't constants of this class.
     *
     * @param string $payload
     * @return string
     */
    public static function decrypt(string $payload): string {
        $payload = json_decode(base64_decode($payload));

        return openssl_decrypt($payload->value, $payload->cipher, $payload->key, OPENSSL_RAW_DATA, $payload->iv);
    }

    /**
     * Encrypt string using the passed cipher and mode.
     * Additional types of ciphers and modes can be used that aren't constants of this class.
     *
     * @param string $string
     * @param string $key
     * @param string $cipher
     * @return string
     */
    public static function encrypt(string $string, string $key, string $cipher): string {
        $iv = static::createIV(16);
        $cipher = static::checkCipher($cipher);
        $value = openssl_encrypt($string, $cipher, $key, OPENSSL_RAW_DATA, $iv);

        return base64_encode(json_encode([
            'iv' => $iv,
            'key' => $key,
            'cipher' => $cipher,
            'value' => $value
        ]));
    }

    /**
     * Return a hashed string using one of the built in ciphers (md5, sha1, sha256, etc) and use the config salt if it has been set.
     * Can also supply an optional second salt for increased security.
     *
     * @param string $cipher
     * @param string $string
     * @param string $salt
     * @return string
     */
    public static function hash(string $cipher, string $string, string $salt = self::SALT): string {
        return hash_hmac($cipher, $string, $salt);
    }

    /**
     * Scrambles the source of a string.
     *
     * @param string $string
     * @return string
     */
    public static function obfuscate(string $string): string {
        $length = mb_strlen($string);
        $scrambled = '';

        if ($length > 0) {
            for ($i = 0; $i < $length; $i++) {
                $scrambled .= '&#' . ord($string[$i]) . ';';
            }
        }

        return $scrambled;
    }

}
