<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Http\Bag;

use Titon\Common\Bag\AbstractBag;
use Titon\Http\Cookie;
use Titon\Utility\State\GlobalMap;

/**
 * Bag for interacting with cookies.
 *
 * @package Titon\Http\Bag
 */
class CookieBag extends AbstractBag<string, Cookie> {

    /**
     * Instantiate a new Cookie class for every cookie in the map.
     *
     * @param \Titon\Utility\State\GlobalMap $data
     */
    public function __construct(GlobalMap $data) {
        $map = Map {};

        foreach ($data as $key => $value) {
            $map[$key] = new Cookie($key, (string) $value);
        }

        parent::__construct($map);
    }

}