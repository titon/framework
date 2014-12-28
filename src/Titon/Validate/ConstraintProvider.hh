<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Validate;

/**
 * Provides a mapping of constraint callbacks to be used in the validator.
 *
 * @package Titon\Validate
 */
interface ConstraintProvider {

    /**
     * Return a map of constraint callbacks with the key being the rule name.
     *
     * @return \Titon\Validate\ConstraintMap
     */
    public function getConstraints(): ConstraintMap;

}