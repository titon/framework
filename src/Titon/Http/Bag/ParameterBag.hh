<?hh // strict
/**
 * @copyright   2010-2014, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Http\Bag;

use Titon\Common\Bag\AbstractBag;
use Titon\Utility\Converter;

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
        return Converter::toArray($this->all());
    }

}