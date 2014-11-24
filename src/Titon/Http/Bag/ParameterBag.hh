<?hh // strict
/**
 * @copyright   2010-2014, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Http\Bag;

use Titon\Common\Bag\AbstractBag;
use Titon\Http\IncomingRequestAware;
use Titon\Utility\Converter;

/**
 * Bag for interacting with request parameters.
 *
 * @package Titon\Http\Bag
 * @codeCoverageIgnore
 */
class ParameterBag<string, mixed> extends AbstractBag {
    use IncomingRequestAware;

    /**
     * Set the parameters using an array. This is because all super globals are arrays.
     *
     * @param array $data
     */
    public function __construct(array $data = []) {
        $this->add(Converter::toMap($data));
    }

}