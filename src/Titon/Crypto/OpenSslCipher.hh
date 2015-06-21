<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Crypto;

use Titon\Crypto\Exception\DecryptException;
use Titon\Crypto\Exception\EncryptException;

/**
 * @todo
 *
 * @package Titon\Crypto
 */
class OpenSslCipher extends AbstractCipher {

    const string AES_128_CFB1 = 'AES-128-CFB1';
    const string AES_128_CFB8 = 'AES-128-CFB8';
    const string AES_192_CFB1 = 'AES-192-CFB1';
    const string AES_192_CFB8 = 'AES-192-CFB8';
    const string AES_256_CFB1 = 'AES-256-CFB1';
    const string AES_256_CFB8 = 'AES-256-CFB8';

    /**
     * {@inheritdoc}
     */
    public function decrypt(string $payload): mixed {
        $payload = $this->decodePayload($payload);
        $method = $this->getMethod();
        $value = openssl_decrypt(hex2bin($payload['data']), $method, $this->getKey(), OPENSSL_RAW_DATA, hex2bin($payload['iv']));

        if ($value === false) {
            throw new DecryptException(sprintf('Decryption with [%s] method has failed: %s', $method, openssl_error_string()));
        }

        return unserialize($value);
    }

    /**
     * {@inheritdoc}
     */
    public function encrypt(mixed $data, bool $strong = false): string {
        $strength = false;
        $method = $this->getMethod();
        $ivLength = openssl_cipher_iv_length($method);
        $iv = openssl_random_pseudo_bytes($ivLength === false ? 16 : $ivLength, $strength);

        if ($iv === false || $strong && !$strength) {
            throw new EncryptException(sprintf('Failed to generate a strong IV for [%s]: %s', $method, openssl_error_string()));
        }

        $value = openssl_encrypt(serialize($data), $method, $this->getKey(), OPENSSL_RAW_DATA, $iv);

        if ($value === false) {
            throw new EncryptException(sprintf('Encryption with [%s] method has failed: %s', $method, openssl_error_string()));
        }

        return $this->encodePayload(bin2hex($value), bin2hex($iv));
    }

    /**
     * {@inheritdoc}
     */
    public static function getSupportedMethods(): Vector<string> {
        return (Vector {
            self::AES_128_CFB1,
            self::AES_128_CFB8,
            self::AES_192_CFB1,
            self::AES_192_CFB8,
            self::AES_256_CFB1,
            self::AES_256_CFB8
        })->addAll(parent::getSupportedMethods());
    }

}
