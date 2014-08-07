<?hh // strict
/**
 * @copyright   2010-2014, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Http\Bag;

use Titon\Http\RequestAware;
use Titon\Utility\Inflector;

/**
 * Bag for interacting with request and response headers.
 * Keys should be case-insensitive, so format each call.
 *
 * @package Titon\Http\Bag
 */
class HeaderBag extends ParameterBag {
    use RequestAware;

    /**
     * {@inheritdoc}
     */
    public function get(string $key, ?mixed $default = null): ?mixed {
        return parent::get($this->key($key), $default);
    }

    /**
     * {@inheritdoc}
     */
    public function has(string $key): bool {
        return parent::has($this->key($key));
    }

    /**
     * Convert keys to the correct title case format.
     *
     * @param string $key
     * @return string
     */
    public function key(string $key): string {
        $key = str_replace([' ', '-', '_'], '-', Inflector::titleCase($key));

        // Special cases
        if ($key === 'Etag') {
            return 'ETag';
        } else if ($key === 'Www-Authenticate') {
            return 'WWW-Authenticate';
        }

        return $key;
    }

    /**
     * {@inheritdoc}
     */
    public function remove(string $key): this {
        return parent::remove($this->key($key));
    }

    /**
     * {@inheritdoc}
     */
    public function set(string $key, ?mixed $value = null): this {
        return parent::set($this->key($key), $value);
    }

}