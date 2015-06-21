<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Crypto;

use Titon\Crypto\Exception\InvalidKeyException;
use Titon\Crypto\Exception\InvalidPayloadException;
use Titon\Crypto\Exception\UnsupportedCipherException;

/**
 * @todo
 *
 * @package Titon\Crypto
 */
abstract class AbstractCipher implements Cipher {

    /**
     * The cipher method.
     *
     * @var string
     */
    protected string $method;

    /**
     * The unique key.
     *
     * @var string
     */
    protected string $key;

    /**
     * Store the key and cipher method and verify its support.
     *
     * @param string $key
     * @param string $method
     */
    public function __construct(string $key, string $method) {
        if (!static::isSupportedMethod($method)) {
            throw new UnsupportedCipherException(sprintf('Unsupported cipher [%s] for [%s]', $method, static::class));

        } else if (!static::isValidKey($key)) {
            throw new InvalidKeyException(sprintf('Invalid key [%s] for [%s], must be 32 characters long', $key, static::class));
        }

        $this->key = $key;
        $this->method = $method;
    }

    /**
     * {@inheritdoc}
     */
    public function decodePayload(string $payload): Payload {
        $payload = json_decode(base64_decode($payload), true);

        if (json_last_error() !== JSON_ERROR_NONE) {
            throw new InvalidPayloadException('Failed to decode payload');
        }

        if (!array_key_exists('iv', $payload) || !$payload['iv']) {
            throw new InvalidPayloadException('Invalid IV detected while decoding payload');
        }

        if (!array_key_exists('data', $payload) || !$payload['data']) {
            throw new InvalidPayloadException('No data detected while decoding payload');
        }

        return shape(
            'iv' => $payload['iv'],
            'data' => $payload['data']
        );
    }

    /**
     * {@inheritdoc}
     */
    public function encodePayload(string $data, string $iv): string {
        return base64_encode(json_encode([
            'iv' => $iv,
            'data' => $data
        ]));
    }

    /**
     * {@inheritdoc}
     */
    public function getKey(): string {
        return $this->key;
    }

    /**
     * {@inheritdoc}
     */
    public function getMethod(): string {
        return $this->method;
    }

    /**
     * {@inheritdoc}
     */
    public static function isValidKey(string $key): bool {
        return (mb_strlen($key, '8bit') >= 32);
    }

}
