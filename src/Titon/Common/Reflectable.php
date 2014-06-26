<?php
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Common;

use Titon\Common\Bag\ReflectionBag;

/**
 * Provides a reflection layer within classes.
 *
 * @package Titon\Common
 */
trait Reflectable {

    /**
     * The reflection bag.
     *
     * @type \Titon\Common\Bag\ReflectionBag
     */
    private $__reflection;

    /**
     * Return the reflection bag.
     *
     * @return \Titon\Common\Bag\ReflectionBag
     */
    public function getReflectionBag() {
        if (!$this->__reflection) {
            $this->__reflection = new ReflectionBag($this);
        }

        return $this->__reflection;
    }

    /**
     * Return reflection information by key.
     *
     * @param string $key
     * @return mixed
     */
    public function reflect($key) {
        return $this->getReflectionBag()->get($key);
    }

}