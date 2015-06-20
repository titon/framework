<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Crypt;

class OpenSslCipher extends AbstractCipher {

    public function decrypt(string $payload): string {
        $payload = json_decode(base64_decode($payload));

        return openssl_decrypt($payload->value, $this->getMethod(), $this->getKey(), OPENSSL_RAW_DATA, $payload->iv);
    }

    public function encrypt(string $string): string {
        $iv = openssl_random_pseudo_bytes(16);
        $value = openssl_encrypt($string, $this->getMethod(), $this->getKey(), OPENSSL_RAW_DATA, $iv);

        return base64_encode(json_encode([
            'iv' => $iv,
            'value' => $value
        ]));
    }

    public static function isSupported(string $key, string $method): bool {
        return in_array($method, openssl_get_cipher_methods());
    }

}
