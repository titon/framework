<?hh // strict
/**
 * @copyright   2010-2014, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Http\Bag;

use Titon\Common\Bag\AbstractBag;

/**
 * Bag for interacting with request parameters.
 *
 * @package Titon\Http\Bag
 * @codeCoverageIgnore
 */
class ParameterBag<string, mixed> extends AbstractBag {

    /**
     * Set the parameter data.
     *
     * @param Map $data
     */
    public function __construct(Map $data = Map {}) {
        $this->add($data);
    }

}