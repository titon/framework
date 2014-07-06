<?hh // strict
/**
 * @copyright   2010-2014, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Common\Bag;

use Titon\Common\Bag;
use Titon\Common\Base;
use Titon\Common\Mutable;
use \IteratorAggregate;
use \Countable;

/**
 * Provides shared functionality for all bags.
 *
 * @package Titon\Common\Bag
 */
abstract class AbstractBag extends Base implements Bag, IteratorAggregate, Countable {
    use Mutable;

    /**
     * Set the parameters.
     *
     * @param Map<string, mixed> $data
     */
    public function __construct(Map<string, mixed> $data = Map {}) {
        $this->add($data);
    }

}