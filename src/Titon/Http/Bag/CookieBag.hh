<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Http\Bag;

use Titon\Common\Bag\AbstractBag;
use Titon\Http\Cookie;

/**
 * Bag for interacting with cookies.
 *
 * @package Titon\Http\Bag
 */
class CookieBag extends AbstractBag<string, Cookie> {

    /**
     * Instantiate a new Cookie class for every cookie in the map.
     *
     * @param Map<string, string> $data
     */
    public function __construct(Map<string, string> $data = Map {}) {
        foreach ($data as $key => $value) {
            $this->set($key, new Cookie($key, $value));
        }
    }

}