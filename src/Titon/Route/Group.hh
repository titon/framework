<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Route;

use Titon\Route\Mixin\ConditionMixin;
use Titon\Route\Mixin\FilterMixin;
use Titon\Route\Mixin\MethodMixin;
use Titon\Route\Mixin\PatternMixin;
use Titon\Route\Mixin\SecureMixin;

/**
 * The Group is used in the defining and passing of information from the group to every Route within the group.
 * A group is triggered from the `Router::group()` method.
 *
 * @package Titon\Route
 */
class Group {
    use ConditionMixin, FilterMixin, MethodMixin, PatternMixin, SecureMixin;

    /**
     * Prefix to prepend to all route paths.
     *
     * @var string
     */
    protected string $prefix = '';

    /**
     * Suffix to append to all route paths.
     *
     * @var string
     */
    protected string $suffix = '';

    /**
     * Return the prefix.
     *
     * @return string
     */
    public function getPrefix(): string {
        return $this->prefix;
    }

    /**
     * Return the suffix.
     *
     * @return string
     */
    public function getSuffix(): string {
        return $this->suffix;
    }

    /**
     * Set the prefix.
     *
     * @param string $prefix
     * @return $this
     */
    public function setPrefix(string $prefix): this {
        $this->prefix = $prefix;

        return $this;
    }

    /**
     * Set the suffix.
     *
     * @param string $suffix
     * @return $this
     */
    public function setSuffix(string $suffix): this {
        $this->suffix = $suffix;

        return $this;
    }

}
