<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Http\Bag;

use Titon\Common\Bag\AbstractBag;
use Titon\Utility\Inflect;

/**
 * Bag for interacting with HTTP headers. Keys should be case-insensitive.
 *
 * @package Titon\Http\Bag
 */
class HeaderBag extends AbstractBag<string, array<string>> {

    /**
     * Append a value to the array of a specific header.
     *
     * @param string $key
     * @param string $value
     * @return $this
     */
    public function append(string $key, string $value): this {
        $array = $this->get($key) ?: [];
        $array[] = $value;

        invariant(is_array($array), 'Header value must be an array.');

        $this->set($key, $array);

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function get(string $key, ?array<string> $default = []): ?array<string> {
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
    <<__Memoize>>
    public function key(string $key): string {
        $key = str_replace([' ', '-', '_'], '-', Inflect::titleCase($key));

        // Special cases
        if ($key === 'Etag') {
            $key = 'ETag';
        } else if ($key === 'Www-Authenticate') {
            $key = 'WWW-Authenticate';
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
    public function set(string $key, array<string> $value): this {
        return parent::set($this->key($key), array_map(fun('trim'), $value));
    }

}
