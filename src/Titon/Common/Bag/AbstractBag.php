<?php
/**
 * @copyright   2010-2014, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Common\Bag;

use Titon\Common\Bag;
use Titon\Common\Base;
use Titon\Common\Mixin\Mutable;
use \ArrayAccess;
use \IteratorAggregate;
use \Countable;

/**
 * Provides shared functionality for all bags.
 *
 * @package Titon\Common\Bag
 */
abstract class AbstractBag extends Base implements Bag, ArrayAccess, IteratorAggregate, Countable {
    use Mutable;

    /**
     * Set the parameters.
     *
     * @param array $data
     */
    public function __construct(array $data = []) {
        $this->add($data);
    }

}