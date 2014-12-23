<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Http\Bag;

use Titon\Common\Bag\AbstractBag;
use Titon\Utility\Col;

/**
 * Bag for interacting with request parameters.
 *
 * @package Titon\Http\Bag
 */
class ParameterBag extends AbstractBag<string, mixed> {

    /**
     * Recursively convert the parameter maps/vectors to an array.
     *
     * @return array<Tk, Tv>
     */
    public function toArray(): array<string, mixed> {
        return Col::toArray($this->all());
    }

}