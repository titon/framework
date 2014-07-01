<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Utility;

use Titon\Common\Macroable;

/**
 * Crypt provides methods for encrypting and decrypting data using popular algorithms like Rijndael, Blowfish and DES.
 * Also provides convenience methods for basic hashing and obfuscation that utilizes configuration settings like salts.
 *
 * @package Titon\Utility
 */
class Crypt {
    use Macroable;

    /**
     * Default ciphers.
     */
    const string BLOWFISH = MCRYPT_BLOWFISH;
    const string DES = MCRYPT_DES;
    const string RIJNDAEL = MCRYPT_RIJNDAEL_128; // AES compliant
    const string TRIPLEDES = MCRYPT_3DES;

    /**
     * Operations.
     */
    const int DECRYPT = MCRYPT_DECRYPT;
    const int ENCRYPT = MCRYPT_ENCRYPT;

    /**
     * Framework salt.
     */
    const string SALT = 'dDFUMG4gZlI0bTNXMFJr';

    /**
     * Encrypt and decrypt a string using the Blowfish algorithm with CBC mode.
     *
     * @param string $string
     * @param string $key
     * @param int $operation
     * @return string
     */
    public static function blowfish(string $string, string $key, int $operation = self::ENCRYPT): string {
        if ($operation === self::ENCRYPT) {
            return static::encrypt($string, $key, self::BLOWFISH);
        } else {
            return static::decrypt($string, $key, self::BLOWFISH);
        }
    }

    /**
     * Decrypt an encrypted string using the passed cipher and mode.
     * Additional types of ciphers and modes can be used that aren't constants of this class.
     *
     * @param string $string
     * @param string $key
     * @param string $cipher
     * @param string $mode
     * @return string
     */
    public static function decrypt(string $string, string $key, string $cipher, string $mode = MCRYPT_MODE_CBC): string {
        list($key, $iv) = static::vector($key, $cipher, $mode);

        return rtrim(mcrypt_decrypt($cipher, $key, $string, $mode, $iv), "\0");
    }

    /**
     * Encrypt and decrypt a string using the DES (data encryption standard) algorithm with CBC mode.
     *
     * @param string $string
     * @param string $key
     * @param int $operation
     * @return string
     */
    public static function des(string $string, string $key, int $operation = self::ENCRYPT): string {
        if ($operation === self::ENCRYPT) {
            return static::encrypt($string, $key, self::DES);
        } else {
            return static::decrypt($string, $key, self::DES);
        }
    }

    /**
     * Encrypt string using the passed cipher and mode.
     * Additional types of ciphers and modes can be used that aren't constants of this class.
     *
     * @param string $string
     * @param string $key
     * @param string $cipher
     * @param string $mode
     * @return string
     */
    public static function encrypt(string $string, string $key, string $cipher, string $mode = MCRYPT_MODE_CBC): string {
        list($key, $iv) = static::vector($key, $cipher, $mode);

        return mcrypt_encrypt($cipher, $key, $string, $mode, $iv);
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
        $string = (string) $string;
        $length = mb_strlen($string);
        $scrambled = '';

        if ($length > 0) {
            for ($i = 0; $i < $length; $i++) {
                $scrambled .= '&#' . ord($string[$i]) . ';';
            }
        }

        return $scrambled;
    }

    /**
     * Encrypt and decrypt a string using the Rijndael 128 algorithm with CBC mode.
     *
     * @param string $string
     * @param string $key
     * @param int $operation
     * @return string
     */
    public static function rijndael(string $string, string $key, int $operation = self::ENCRYPT): string {
        if ($operation === self::ENCRYPT) {
            return static::encrypt($string, $key, self::RIJNDAEL);
        } else {
            return static::decrypt($string, $key, self::RIJNDAEL);
        }
    }

    /**
     * Encrypt and decrypt a string using the 3DES (triple data encryption standard) algorithm with CBC mode.
     *
     * @param string $string
     * @param string $key
     * @param int $operation
     * @return string
     */
    public static function tripledes(string $string, string $key, int $operation = self::ENCRYPT): string {
        if ($operation === self::ENCRYPT) {
            return static::encrypt($string, $key, self::TRIPLEDES);
        } else {
            return static::decrypt($string, $key, self::TRIPLEDES);
        }
    }

    /**
     * Prepare for en/decryption by generating persistent keys and IVs.
     * We can't use randomization as the key/iv needs to be the same for both encrypt and decrypt.
     *
     * @param string $key
     * @param string $cipher
     * @param string $mode
     * @return Pair
     */
    public static function vector(string $key, string $cipher, string $mode): Pair {
        $keySize = mcrypt_get_key_size($cipher, $mode);
        $key = str_pad(static::hash('md5', $key), $keySize, mb_substr($cipher, -1), STR_PAD_BOTH);

        if (mb_strlen($key) > $keySize) {
            $key = mb_substr($key, 0, $keySize);
        }

        $ivSize = mcrypt_get_iv_size($cipher, $mode);
        $iv = str_pad(static::hash('md5', $key, $mode), $ivSize, mb_substr($cipher, 0, 1), STR_PAD_BOTH);

        if (mb_strlen($iv) > $ivSize) {
            $iv = mb_substr($iv, 0, $ivSize);
        }

        return Pair {$key, $iv};
    }

}