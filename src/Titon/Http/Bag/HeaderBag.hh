<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Http\Bag;

use Titon\Common\Bag\AbstractBag;
use Titon\Utility\Col;
use Titon\Utility\Inflector;

/**
 * Bag for interacting with request and response headers.
 * Keys should be case-insensitive.
 *
 * @package Titon\Http\Bag
 */
class HeaderBag<Tk, Tv> extends AbstractBag<Tk, Tv> {

    /**
     * {@inheritdoc}
     */
    public function get(Tk $key, ?Tv $default = null): ?Tv {
        return parent::get($this->key($key), $default);
    }

    /**
     * {@inheritdoc}
     */
    public function has(Tk $key): bool {
        return parent::has($this->key($key));
    }

    /**
     * Convert keys to the correct title case format.
     *
     * @param Tk $key
     * @return Tk
     */
    public function key(Tk $key): Tk {
        $key = str_replace([' ', '-', '_'], '-', Inflector::titleCase((string) $key));

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
    public function remove(Tk $key): this {
        return parent::remove($this->key($key));
    }

    /**
     * {@inheritdoc}
     */
    public function set(Tk $key, Tv $value, bool $add = false): this {
        if (!$add) {
            return parent::set($this->key($key), Col::toArray($value));
        }

        $list = $this->get($key, []);

        invariant(is_array($list), 'Value must be an array');

        $list[] = $value;

        return parent::set($this->key($key), $list);
    }

}