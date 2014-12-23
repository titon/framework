<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Route\Mixin;

use Titon\Route\Route;

type ConditionCallback = (function(Route): bool);
type ConditionList = Vector<ConditionCallback>;

/**
 * Provides functionality for conditionals.
 *
 * @package Titon\Route\Mixin
 */
trait ConditionMixin {

    /**
     * List of conditions to validate against.
     *
     * @var \Titon\Route\Mixin\ConditionList
     */
    protected ConditionList $_conditions = Vector {};

    /**
     * Add a condition callback.
     *
     * @param \Titon\Route\Mixin\ConditionCallback $condition
     * @return $this
     */
    public function addCondition(ConditionCallback $condition): this {
        $this->_conditions[] = $condition;

        return $this;
    }

    /**
     * Add multiple conditions.
     *
     * @param \Titon\Route\Mixin\ConditionList $conditions
     * @return $this
     */
    public function addConditions(ConditionList $conditions): this {
        foreach ($conditions as $condition) {
            $this->addCondition($condition);
        }

        return $this;
    }

    /**
     * Return the list of conditions.
     *
     * @return \Titon\Route\Mixin\ConditionList
     */
    public function getConditions(): ConditionList {
        return $this->_conditions;
    }

    /**
     * Set the list of conditions to validate.
     *
     * @param \Titon\Route\Mixin\ConditionList $conditions
     * @return $this
     */
    public function setConditions(ConditionList $conditions): this {
        $this->_conditions = $conditions;

        return $this;
    }

}