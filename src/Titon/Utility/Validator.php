<?php
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Utility;

use Titon\Utility\Exception\InvalidArgumentException;
use Titon\Utility\Exception\InvalidValidationRuleException;

/**
 * The Validator allows for quick validation against a defined set of rules and fields.
 *
 * @package Titon\Utility
 */
class Validator {

    /**
     * Data to validate against.
     *
     * @type array
     */
    protected $_data = array();

    /**
     * Errors gathered during validation.
     *
     * @type array
     */
    protected $_errors = array();

    /**
     * Mapping of fields and titles.
     *
     * @type array
     */
    protected $_fields = array();

    /**
     * Fallback mapping of error messages.
     *
     * @type array
     */
    protected $_messages = array();

    /**
     * Mapping of fields and validation rules.
     *
     * @type array
     */
    protected $_rules = array();

    /**
     * Store the data to validate.
     *
     * @param array $data
     */
    public function __construct(array $data = array()) {
        $this->setData($data);
    }

    /**
     * Mark a field has an error.
     *
     * @param string $field
     * @param string $message
     * @return $this
     */
    public function addError($field, $message) {
        $this->_errors[$field] = $message;

        return $this;
    }

    /**
     * Add a field to be used in validation. Can optionally apply an array of validation rules.
     *
     * @param string $field
     * @param string $title
     * @param array $rules
     * @return $this
     */
    public function addField($field, $title, array $rules = array()) {
        $this->_fields[$field] = $title;

        /**
         * 0 => rule
         * rule => [opt, ...]
         */
        if ($rules) {
            foreach ($rules as $rule => $options) {
                if (is_numeric($rule)) {
                    $rule = $options;
                    $options = array();
                }

                $this->addRule($field, $rule, null, $options);
            }
        }

        return $this;
    }

    /**
     * Add messages to the list.
     *
     * @param array $messages
     * @return $this
     */
    public function addMessages(array $messages) {
        $this->_messages = array_replace($this->_messages, $messages);

        return $this;
    }

    /**
     * Add a validation rule to a field. Can supply an optional error message and options.
     *
     * @param string $field
     * @param string $rule
     * @param string $message
     * @param array $options
     * @return $this
     * @throws \Titon\Utility\Exception\InvalidArgumentException
     */
    public function addRule($field, $rule, $message, $options = array()) {
        if (empty($this->_fields[$field])) {
            throw new InvalidArgumentException(sprintf('Field %s does not exist', $field));
        }

        if (isset($this->_messages[$rule])) {
            $message = $message ?: $this->_messages[$rule];
        } else {
            $this->_messages[$rule] = $message;
        }

        $this->_rules[$field][$rule] = array(
            'message' => $message,
            'options' => (array) $options
        );

        return $this;
    }

    /**
     * Return the currently set data.
     *
     * @return array
     */
    public function getData() {
        return $this->_data;
    }

    /**
     * Return the errors.
     *
     * @return array
     */
    public function getErrors() {
        return $this->_errors;
    }

    /**
     * Return the fields.
     *
     * @return array
     */
    public function getFields() {
        return $this->_fields;
    }

    /**
     * Return the messages.
     *
     * @return array
     */
    public function getMessages() {
        return $this->_messages;
    }

    /**
     * Return the rules.
     *
     * @return array
     */
    public function getRules() {
        return $this->_rules;
    }

    /**
     * Reset all dynamic properties.
     *
     * @return $this
     */
    public function reset() {
        $this->_data = array();
        $this->_errors = array();

        return $this;
    }

    /**
     * Set the data to validate against.
     *
     * @param array $data
     * @return $this
     */
    public function setData(array $data) {
        $this->_data = $data;

        return $this;
    }

    /**
     * Validate the data against the rules schema. Return true if all fields passed validation.
     *
     * @return bool
     * @throws \Titon\Utility\Exception\InvalidValidationRuleException
     */
    public function validate() {
        if (!$this->_data) {
            return false;
        }

        $fields = $this->getFields();
        $messages = $this->getMessages();

        foreach ($this->_data as $field => $value) {
            if (empty($this->_rules[$field])) {
                continue;
            }

            foreach ($this->_rules[$field] as $rule => $params) {
                $options = $params['options'];
                $arguments = $options;
                array_unshift($arguments, $value);

                // Use G11n if it is available
                if (class_exists('Titon\G11n\Utility\Validate')) {
                    $class = 'Titon\G11n\Utility\Validate';
                } else {
                    $class = 'Titon\Utility\Validate';
                }

                if (!call_user_func(array($class, 'hasMethod'), $rule)) {
                    throw new InvalidValidationRuleException(sprintf('Validation rule %s does not exist', $rule));
                }

                // Prepare messages
                $message = $params['message'];

                if (!$message && isset($messages[$rule])) {
                    $message = $messages[$rule];
                }

                if ($message) {
                    $message = String::insert($message, array_map(function($value) {
                        return is_array($value) ? implode(', ', $value) : $value;
                    }, $options + array(
                        'field' => $field,
                        'title' => $fields[$field]
                    )));
                } else {
                    throw new InvalidValidationRuleException(sprintf('Error message for rule %s does not exist', $rule));
                }

                if (!call_user_func_array(array($class, $rule), $arguments)) {
                    $this->addError($field, $message);
                    break;
                }
            }
        }

        return empty($this->_errors);
    }

    /**
     * Create a validator instance from a set of shorthand or expanded rule sets.
     *
     * @param array $data
     * @param array $fields
     * @return $this
     */
    public static function makeFromShorthand(array $data = array(), array $fields = array()) {
        /** @type \Titon\Utility\Validator $obj */
        $obj = new static($data);

        foreach ($fields as $field => $data) {
            $title = $field;

            // Convert to array
            if (is_string($data)) {
                $data = array('rules' => $data);

            } else if (!is_array($data)) {
                continue;

            } else if (Hash::isNumeric(array_keys($data))) {
                $data = array('rules' => $data);
            }

            // Prepare for parsing
            if (isset($data['title'])) {
                $title = $data['title'];
            }

            if (is_string($data['rules'])) {
                $data['rules'] = explode('|', $data['rules']);
            }

            $obj->addField($field, $title);

            foreach ($data['rules'] as $ruleOpts) {
                $shorthand = self::splitShorthand($ruleOpts);

                $obj->addRule($field, $shorthand['rule'], $shorthand['message'], $shorthand['options']);
            }
        }

        return $obj;
    }

    /**
     * Split a shorthand rule into multiple parts.
     *
     * @param string $shorthand
     * @return array
     */
    public static function splitShorthand($shorthand) {
        $rule = null;
        $message = '';
        $opts = array();

        // rule:o1,o2,o3
        // rule:o1,o2:The message here!
        if (strpos($shorthand, ':') !== false) {
            $parts = explode(':', $shorthand, 3);
            $rule = $parts[0];

            if (!empty($parts[1])) {
                $opts = $parts[1];

                if (strpos($opts, ',') !== false) {
                    $opts = explode(',', $opts);
                } else {
                    $opts = array($opts);
                }
            }

            if (!empty($parts[2])) {
                $message = $parts[2];
            }

        // rule
        } else {
            $rule = $shorthand;
        }

        return array(
            'rule' => $rule,
            'message' => $message,
            'options' => $opts
        );
    }

}