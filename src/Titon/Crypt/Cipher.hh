<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Crypt;

interface Cipher {

    public function decrypt(string $payload): string;

    public function encrypt(string $string): string;

    public function getKey(): string;

    public function getMethod(): string;

    public static function isSupported(string $key, string $method): bool;

}
