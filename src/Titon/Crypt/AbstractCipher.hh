<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Crypt;

use Titon\Crypt\Exception\UnsupportedCipherException;

/**
 * @todo
 *
 * @package Titon\Crypt
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
        if (!static::isSupported($key, $method)) {
            throw new UnsupportedCipherException(sprintf('Unsupported cipher [%s] for [%s]', $method, static::class));
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
