<?hh // strict
/**
 * @copyright   2010-2014, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

/**
 * Return true if the value passed is traversable.
 * Either an array, map, vector, or class that implements the Traversable interface.
 *
 * @param mixed $value
 * @return bool
 */
if (!function_exists('is_traversable')) {
    function is_traversable(?mixed $value): bool {
        return (is_array($value) || $value instanceof Traversable);
    }
}