<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
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
     * @var bool
     */
    protected bool $secure = false;

    /**
     * Return the secure configuration.
     *
     * @return bool
     */
    public function getSecure(): bool {
        return $this->secure;
    }

    /**
     * Set the secure flag.
     *
     * @param bool $secure
     * @return $this
     */
    public function setSecure(bool $secure): this {
        $this->secure = $secure;

        return $this;
    }

}
