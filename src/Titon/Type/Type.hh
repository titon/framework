<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Type;

/**
 * Provides very low level helper functionality for types.
 *
 * @package Titon\Type
 */
class Type {

    /**
     * Returns a type name represented as a string.
     *
     * @param mixed $data
     * @return string
     */
    public static function is(mixed $data): string {
        if (is_array($data)) {
            return 'array';

        } else if ($data instanceof Map) {
            return 'map';

        } else if ($data instanceof Vector) {
            return 'vector';

        } else if ($data instanceof Set) {
            return 'set';

        } else if ($data instanceof Pair) {
            return 'pair';

        } else if (is_callable($data)) {
            return 'callable';

        } else if (is_object($data)) {
            return 'object';
        }

        // Attempt other types
        return strtolower(gettype($data));
    }

}
