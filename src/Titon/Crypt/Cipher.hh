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
interface Cipher {

    /**
     * Decrypt the payload using the defined cipher method and unique key
     * to return the original value.
     *
     * @param string $payload
     * @return mixed
     */
    public function decrypt(string $payload): mixed;

    /**
     * Encrypt the string using the defined cipher method and unique key.
     * A payload will be returned that can be decrypted for the original value.
     *
     * @param mixed $string
     * @return string
     */
    public function encrypt(mixed $string): string;

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
     * Verify the chosen cipher method is supported.
     *
     * @param string $key
     * @param string $method
     * @return bool
     */
    public static function isSupported(string $key, string $method): bool;

}
