<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Validate;

/**
 * The Validator allows for quick validation against a defined set of rules and fields.
 *
 * @package Titon\Validate
 */
interface Validator {

    /**
     * Add a custom constraint.
     *
     * @param string $key
     * @param \Titon\Validate\ConstraintCallback $callback
     * @return $this
     */
    public function addConstraint(string $key, ConstraintCallback $callback): this;

    /**
     * Load a mapping of constraints defined in a provider.
     *
     * @param \Titon\Validate\ConstraintProvider $provider
     * @return $this
     */
    public function addConstraintProvider(ConstraintProvider $provider): this;

    /**
     * Mark a field has an error.
     *
     * @param string $field
     * @param string $message
     * @return $this
     */
    public function addError(string $field, string $message): this;

    /**
     * Add a field to be used in validation. Can optionally apply an array of validation rules.
     *
     * @param string $field
     * @param string $title
     * @param Map<string, OptionList> $rules
     * @return $this
     */
    public function addField(string $field, string $title, Map<string, OptionList> $rules = Map {}): this;

    /**
     * Add messages to the list.
     *
     * @param \Titon\Validate\MessageMap $messages
     * @return $this
     */
    public function addMessages(MessageMap $messages): this;

    /**
     * Add a validation rule to a field. Can supply an optional error message and options.
     *
     * @param string $field
     * @param string $rule
     * @param string $message
     * @param \Titon\Validate\OptionList $options
     * @return $this
     */
    public function addRule(string $field, string $rule, string $message, OptionList $options = Vector{}): this;

    /**
     * Return a map of constraint callbacks with the key being the rule name.
     *
     * @return \Titon\Validate\ConstraintMap
     */
    public function getConstraints(): ConstraintMap;

    /**
     * Return the currently set data.
     *
     * @return \Titon\Validate\DataMap
     */
    public function getData(): DataMap;

    /**
     * Return the errors.
     *
     * @return \Titon\Validate\ErrorMap
     */
    public function getErrors(): ErrorMap;

    /**
     * Return the fields.
     *
     * @return \Titon\Validate\FieldMap
     */
    public function getFields(): FieldMap;

    /**
     * Return the messages.
     *
     * @return \Titon\Validate\MessageMap
     */
    public function getMessages(): MessageMap;

    /**
     * Return the rules.
     *
     * @return \Titon\Validate\RuleContainer
     */
    public function getRules(): RuleContainer;

    /**
     * Reset the state of the validator.
     *
     * @return $this
     */
    public function reset(): this;

    /**
     * Set the data to validate against.
     *
     * @param \Titon\Validate\DataMap $data
     * @return $this
     */
    public function setData(DataMap $data): this;

    /**
     * Validate the data against the rules schema. Return true if all fields passed validation.
     *
     * @param \Titon\Validate\DataMap $data
     * @return bool
     */
    public function validate(DataMap $data = Map {}): bool;

}
