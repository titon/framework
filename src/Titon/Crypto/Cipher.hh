<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Crypto;

/**
 * The `Cipher` interface represents a single cipher method (and mode), paired with a unique key,
 * to be used for encryption and decryption. The process uses a unique payload structure to
 * ensure security and integrity.
 *
 * @package Titon\Crypto
 */
interface Cipher {

    /**
     * Decode the payload string into a container of data and IV.
     * Verify the payload during this process and exit early if an error occurs.
     *
     * @param string $payload
     * @return \Titon\Crypto\Payload
     */
    public function decodePayload(string $payload): Payload;

    /**
     * Decrypt the payload using the defined cipher method and unique key
     * to return the original value.
     *
     * @param string $payload
     * @return mixed
     */
    public function decrypt(string $payload): mixed;

    /**
     * Encode the data and its associated options, like IV, into a usable string payload.
     *
     * @param string $data
     * @param string $iv
     * @return string
     * @throws \Titon\Crypto\Exception\InvalidPayloadException
     */
    public function encodePayload(string $data, string $iv): string;

    /**
     * Encrypt the string using the defined cipher method and unique key.
     * A payload will be returned that can be decrypted for the original value.
     *
     * @param mixed $data
     * @param bool $strong      Verify that the encryption is strong
     * @return string
     */
    public function encrypt(mixed $data, bool $strong = false): string;

    /**
     * Return the unique key.
     *
     * @return string
     */
    public function getKey(): string;

    /**
     * Return the cipher method being used.
     *
     * @return string
     */
    public function getMethod(): string;

    /**
     * Return the cipher mode being used.
     *
     * @return string
     */
    public function getMode(): string;

    /**
     * Return a list of supported cipher methods.
     *
     * @return Vector<string>
     */
    public static function getSupportedMethods(): Vector<string>;

    /**
     * Verify the chosen cipher method is supported.
     *
     * @param string $method
     * @return bool
     */
    public static function isSupportedMethod(string $method): bool;

    /**
     * Verify the unique key is valid.
     *
     * @param string $key
     * @return bool
     */
    public static function isValidKey(string $key): bool;

}
