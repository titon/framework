<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Crypt;

/**
 * @todo
 *
 * @package Titon\Crypt
 */
class OpenSslCipher extends AbstractCipher {

    const string AES_128_CBC = 'AES-128-CBC';
    const string AES_256_CBC = 'AES-256-CBC';

    public function decrypt(string $payload): mixed {
        $payload = json_decode(base64_decode($payload));

        $value = openssl_decrypt($payload->value, $this->getMethod(), $this->getKey(), OPENSSL_RAW_DATA, $payload->iv);

        if ($iv === false) {
            throw new \RuntimeException('todo');
        }

        return unserialize($value);
    }

    public function encrypt(mixed $string): string {
        $iv = openssl_random_pseudo_bytes(16);
        $value = openssl_encrypt(serialize($string), $this->getMethod(), $this->getKey(), OPENSSL_RAW_DATA, $iv);

        if ($iv === false) {
            throw new \RuntimeException('todo');
        }

        return base64_encode(json_encode([
            'iv' => $iv,
            'value' => $value
        ]));
    }

    public static function isSupported(string $key, string $method): bool {
        return in_array($method, openssl_get_cipher_methods());
    }

}
