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
 * The `McryptCipher` provides encryption and decryption through the MCrypt extension.
 * The MCrypt extension is very outdated, and should be avoided where possible.
 *
 * @package Titon\Crypto
 */
class McryptCipher extends AbstractCipher {

    /**
     * {@inheritdoc}
     */
    public function decrypt(string $payload): mixed {
        $payload = $this->decodePayload($payload);
        $value = mcrypt_decrypt($this->getMcryptAlgorithm(), $this->getKey(), hex2bin($payload['data']), strtolower($this->getMode()), hex2bin($payload['iv']));

        if ($value === false) {
            throw new DecryptException(sprintf('Decryption with [%s] method has failed', $this->getMethod()));
        }

        return unserialize($value);
    }

    /**
     * {@inheritdoc}
     */
    public function encrypt(mixed $data, bool $strong = false): string {
        $method = $this->getMcryptAlgorithm();
        $mode = strtolower($this->getMode());
        $ivLength = mcrypt_get_iv_size($method, $mode);
        $iv = mcrypt_create_iv($ivLength === false ? 16 : $ivLength, MCRYPT_DEV_URANDOM);

        if ($iv === false) {
            throw new EncryptException(sprintf('Failed to generate a strong IV for [%s]', $this->getMethod()));
        }

        $value = mcrypt_encrypt($method, $this->getKey(), serialize($data), $mode, $iv);

        if ($value === false) {
            throw new EncryptException(sprintf('Encryption with [%s] method has failed', $this->getMethod()));
        }

        return $this->encodePayload(bin2hex($value), bin2hex($iv));
    }

    /**
     * Return the mcrypt specific cipher algorithm.
     *
     * @return string
     */
    public function getMcryptAlgorithm(): string {
        switch ($this->algorithm) {
            case 'BF': return MCRYPT_BLOWFISH; break;
            case 'AES-128': return MCRYPT_RIJNDAEL_128; break;
            case 'AES-192': return MCRYPT_RIJNDAEL_192; break;
            case 'AES-256': return MCRYPT_RIJNDAEL_256; break;
        }

        return $this->algorithm;
    }

}
