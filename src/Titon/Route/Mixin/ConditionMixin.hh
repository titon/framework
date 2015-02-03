<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Route\Mixin;

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
    protected ConditionList $conditions = Vector {};

    /**
     * Add a condition callback.
     *
     * @param \Titon\Route\Mixin\ConditionCallback $condition
     * @return $this
     */
    public function addCondition(ConditionCallback $condition): this {
        $this->conditions[] = $condition;

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
        return $this->conditions;
    }

    /**
     * Set the list of conditions to validate.
     *
     * @param \Titon\Route\Mixin\ConditionList $conditions
     * @return $this
     */
    public function setConditions(ConditionList $conditions): this {
        $this->conditions = $conditions;

        return $this;
    }

}
