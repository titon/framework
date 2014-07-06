<?hh //strict
/**
 * @copyright   2010-2014, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Http\Bag;

use Titon\Common\Bag\AbstractBag;
use Titon\Http\RequestAware;

/**
 * Bag for interacting with request parameters.
 *
 * @package Titon\Http\Bag
 * @codeCoverageIgnore
 */
class ParameterBag extends AbstractBag {
    use RequestAware;
}