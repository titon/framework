<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Validate;

use Titon\Utility\Validate;

/**
 * Default provider of constraints, which are inherited from the Validate class.
 *
 * @package Titon\Validate
 */
class Constraint extends Validate implements ConstraintProvider {

    /**
     * {@inheritdoc}
     */
    public function getConstraints(): ConstraintMap {
        $constraints = Map {};
        $class = static::class;

        foreach (get_class_methods($this) as $method) {
            // UNSAFE
            // Since class_meth() requires literal strings and we are passing variables
            $constraints[$method] = class_meth($class, $method);
        }

        // Wrap macros in a lambda as callback definitions are different
        foreach (static::getMacros() as $macro => $callback) {
            $constraints[$macro] = ($input) ==> call_user_func_array($callback, func_get_args());
        }

        return $constraints;
    }

}