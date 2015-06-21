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
class McryptCipher extends AbstractCipher {

    /**
     * The original method before conversion.
     *
     * @var string
     */
    protected string $originalMethod = '';

    /**
     * Convert the method and mode to valid mcrypt values.
     *
     * @param string $key
     * @param string $method
     */
    public function __construct(string $key, string $method) {
        parent::__construct($key, $method);

        $this->originalMethod = $method;

        // Determine the correct mcrypt method
        $method = substr($method, 0, strlen($method) - 4);

        switch ($method) {
            case 'BF': $method = MCRYPT_BLOWFISH; break;
            case 'AES-128': $method = MCRYPT_RIJNDAEL_128; break;
            case 'AES-192': $method = MCRYPT_RIJNDAEL_192; break;
            case 'AES-256': $method = MCRYPT_RIJNDAEL_256; break;
        }

        $this->method = $method;
        $this->mode = strtolower($this->mode);
    }

    /**
     * {@inheritdoc}
     */
    public function decrypt(string $payload): mixed {
        $payload = $this->decodePayload($payload);
        $value = mcrypt_decrypt($this->getMethod(), $this->getKey(), hex2bin($payload['data']), $this->getMode(), hex2bin($payload['iv']));

        if ($value === false) {
            throw new DecryptException(sprintf('Decryption with [%s] method has failed', $this->originalMethod));
        }

        return unserialize($value);
    }

    /**
     * {@inheritdoc}
     */
    public function encrypt(mixed $data, bool $strong = false): string {
        $method = $this->getMethod();
        $mode = $this->getMode();
        $ivLength = mcrypt_get_iv_size($method, $mode);
        $iv = mcrypt_create_iv($ivLength === false ? 16 : $ivLength, MCRYPT_DEV_URANDOM);

        if ($iv === false) {
            throw new EncryptException(sprintf('Failed to generate a strong IV for [%s]', $this->originalMethod));
        }

        $value = mcrypt_encrypt($method, $this->getKey(), serialize($data), $mode, $iv);

        if ($value === false) {
            throw new EncryptException(sprintf('Encryption with [%s] method has failed', $this->originalMethod));
        }

        return $this->encodePayload(bin2hex($value), bin2hex($iv));
    }

}
