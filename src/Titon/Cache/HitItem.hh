<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Cache;

/**
 * This class represents an item that was found in the cache (a hit).
 *
 * @package Titon\Cache
 */
class HitItem extends Item {

    /**
     * {@inheritdoc}
     */
    public function exists(): bool {
        return true;
    }

    /**
     * {@inheritdoc}
     */
    public function isHit(): bool {
        return true;
    }

}