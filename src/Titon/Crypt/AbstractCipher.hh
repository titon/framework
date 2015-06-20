<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Crypt;

use Titon\Crypt\Exception\UnsupportedCipherException;

abstract class AbstractCipher implements Cipher {

    protected string $method;

    protected string $key;

    public function __construct(string $key, string $method) {
        if (!static::isSupported($key, $method)) {
            throw new UnsupportedCipherException(sprintf('Unsupported cipher [%s]', $method));
        }

        $this->key = $key;
        $this->method = $method;
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

}
