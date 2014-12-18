<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Route\Mixin;

/**
 * Provides functionality for security.
 *
 * @package Titon\Route\Mixin
 */
trait SecureMixin {

    /**
     * When true, will only match if under HTTPS.
     *
     * @type bool
     */
    protected bool $_secure = false;

    /**
     * Return the secure configuration.
     *
     * @return bool
     */
    public function getSecure(): bool {
        return $this->_secure;
    }

    /**
     * Set the secure flag.
     *
     * @param bool $secure
     * @return $this
     */
    public function setSecure(bool $secure): this {
        $this->_secure = $secure;

        return $this;
    }

}