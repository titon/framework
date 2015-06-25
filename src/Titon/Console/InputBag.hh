<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Console;

use Titon\Common\Bag\AbstractBag;

/**
 * The `InputBag` holds available `InputDefinition` objects to reference.
 *
 * @package Titon\Console
 */
class InputBag<T as InputDefinition> extends AbstractBag<string, T> {

    /**
     * Retrieve the `InputDefinition` object based on the given key. The key is
     * checked against all available names as well as aliases.
     *
     * @param string $key   The key to match
     * @param T $default    The default value to return if no `InputDefinition`
     *                      is found
     *
     * @return T|null
     */
    public function get(string $key, ?T $default = null): ?T {
        if (is_null($arg = parent::get($key))) {
            foreach ($this as $val) {
                if ($key === $val->getAlias()) {
                    return $val;
                }
            }
        }

        return $default;
    }
}
