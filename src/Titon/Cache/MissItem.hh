<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Cache;

/**
 * This class represents an item that was not found in the cache (a miss).
 *
 * @package Titon\Cache
 */
class MissItem extends Item {

    /**
     * {@inheritdoc}
     */
    public function exists(): bool {
        return false;
    }

}