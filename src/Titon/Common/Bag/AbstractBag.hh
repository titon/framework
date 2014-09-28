<?hh // strict
/**
 * @copyright   2010-2014, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Common\Bag;

use Titon\Common\Bag;
use Titon\Common\Mutable;
use \IteratorAggregate;
use \Countable;

/**
 * Provides shared functionality for all bags.
 *
 * @package Titon\Common\Bag
 */
abstract class AbstractBag<Tk, Tv> implements Bag<Tk, Tv>, IteratorAggregate<Tv>, Countable {
    use Mutable<Tk, Tv>;

    /**
     * Set the parameters.
     *
     * @param Map<Tk, Tv> $data
     */
    public function __construct(Map<Tk, Tv> $data = Map {}) {
        $this->add($data);
    }

}