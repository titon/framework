<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Common;

use Titon\Common\DataMap;
use Titon\Common\Validator\ErrorMap;
use Titon\Common\Validator\FieldMap;
use Titon\Common\Validator\MessageMap;
use Titon\Common\Validator\RuleContainer;
use Titon\Common\Validator\RuleOptionList;

/**
 * The Validator allows for quick validation against a defined set of rules and fields.
 *
 * @package Titon\Common
 */
interface Validator {

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
     * @param Map<string, RuleOptionList> $rules
     * @return $this
     */
    public function addField(string $field, string $title, Map<string, RuleOptionList> $rules = Map {}): this;

    /**
     * Add messages to the list.
     *
     * @param \Titon\Common\Validator\MessageMap $messages
     * @return $this
     */
    public function addMessages(MessageMap $messages): this;

    /**
     * Add a validation rule to a field. Can supply an optional error message and options.
     *
     * @param string $field
     * @param string $rule
     * @param string $message
     * @param \Titon\Common\Validator\RuleOptionList $options
     * @return $this
     */
    public function addRule(string $field, string $rule, string $message, RuleOptionList $options = Vector{}): this;

    /**
     * Return the currently set data.
     *
     * @return \Titon\Common\DataMap
     */
    public function getData(): DataMap;

    /**
     * Return the errors.
     *
     * @return \Titon\Common\Validator\ErrorMap
     */
    public function getErrors(): ErrorMap;

    /**
     * Return the fields.
     *
     * @return \Titon\Common\Validator\FieldMap
     */
    public function getFields(): FieldMap;

    /**
     * Return the messages.
     *
     * @return \Titon\Common\Validator\MessageMap
     */
    public function getMessages(): MessageMap;

    /**
     * Return the rules.
     *
     * @return \Titon\Common\Validator\RuleContainer
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
     * @param \Titon\Common\DataMap $data
     * @return $this
     */
    public function setData(DataMap $data): this;

    /**
     * Validate the data against the rules schema. Return true if all fields passed validation.
     *
     * @return bool
     */
    public function validate(): bool;

}