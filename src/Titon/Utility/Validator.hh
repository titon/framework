<?hh // strict
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
    protected $_data = [];

    /**
     * Errors gathered during validation.
     *
     * @type Map<string, string>
     */
    protected Map<string, string> $_errors = Map {};

    /**
     * Mapping of fields and titles.
     *
     * @type Map<string, string>
     */
    protected Map<string, string> $_fields = Map {};

    /**
     * Fallback mapping of error messages.
     *
     * @type Map<string, string>
     */
    protected Map<string, string> $_messages = Map {};

    /**
     * Mapping of fields and validation rules.
     *
     * @type Map<string, mixed>
     */
    protected Map<string, mixed> $_rules = Map {};

    /**
     * Store the data to validate.
     *
     * @param array $data
     */
    public function __construct(array $data = []) {
        $this->setData($data);
    }

    /**
     * Mark a field has an error.
     *
     * @param string $field
     * @param string $message
     * @return $this
     */
    public function addError(string $field, string $message): this {
        $this->_errors[$field] = $message;

        return $this;
    }

    /**
     * Add a field to be used in validation. Can optionally apply an array of validation rules.
     *
     * @param string $field
     * @param string $title
     * @param Map<mixed, mixed> $rules
     * @return $this
     */
    public function addField(string $field, string $title, Map<mixed, mixed> $rules = Map {}): this {
        $this->_fields[$field] = $title;

        /**
         * 0 => rule
         * rule => [opt, ...]
         */
        if (!$rules->isEmpty()) {
            foreach ($rules as $rule => $options) {
                if (is_numeric($rule)) {
                    $rule = $options;
                    $options = [];
                }

                $this->addRule($field, $rule, null, $options);
            }
        }

        return $this;
    }

    /**
     * Add messages to the list.
     *
     * @param Map<string, string> $messages
     * @return $this
     */
    public function addMessages(Map<string, string> $messages): this {
        $this->_messages->setAll($messages);

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
    public function addRule(string $field, string $rule, string $message, Vector<mixed> $options = Vector{}): this {
        if (empty($this->_fields[$field])) {
            throw new InvalidArgumentException(sprintf('Field %s does not exist', $field));
        }

        if (isset($this->_messages[$rule])) {
            $message = $message ?: $this->_messages[$rule];
        } else {
            $this->_messages[$rule] = $message;
        }

        $this->_rules[$field][$rule] = Map {
            'message' => $message,
            'options' => $options
        };

        return $this;
    }

    /**
     * Return the currently set data.
     *
     * @return array
     */
    public function getData(): array {
        return $this->_data;
    }

    /**
     * Return the errors.
     *
     * @return Map<string, string>
     */
    public function getErrors(): Map<string, string> {
        return $this->_errors;
    }

    /**
     * Return the fields.
     *
     * @return Map<string, string>
     */
    public function getFields(): Map<string, string> {
        return $this->_fields;
    }

    /**
     * Return the messages.
     *
     * @return Map<string, string>
     */
    public function getMessages(): Map<string, string> {
        return $this->_messages;
    }

    /**
     * Return the rules.
     *
     * @return Map<string, mixed>
     */
    public function getRules(): Map<string, mixed> {
        return $this->_rules;
    }

    /**
     * Reset all dynamic properties.
     *
     * @return $this
     */
    public function reset(): this {
        $this->_data->clear();
        $this->_errors->clear();

        return $this;
    }

    /**
     * Set the data to validate against.
     *
     * @param array $data
     * @return $this
     */
    public function setData(array $data): this {
        $this->_data = $data;

        return $this;
    }

    /**
     * Validate the data against the rules schema. Return true if all fields passed validation.
     *
     * @return bool
     * @throws \Titon\Utility\Exception\InvalidValidationRuleException
     */
    public function validate(): bool {
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
                // @codeCoverageIgnoreStart
                if (class_exists('Titon\G11n\Utility\Validate')) {
                    $class = 'Titon\G11n\Utility\Validate';
                } else {
                    $class = 'Titon\Utility\Validate';
                }
                // @codeCoverageIgnoreEnd

                if (!call_user_func([$class, 'hasMethod'], $rule)) {
                    throw new InvalidValidationRuleException(sprintf('Validation rule %s does not exist', $rule));
                }

                // Prepare messages
                $message = $params['message'];

                if (!$message && isset($messages[$rule])) {
                    $message = $messages[$rule];
                }

                if ($message) {
                    $message = Str::insert($message, array_map(function($value) {
                        return is_array($value) ? implode(', ', $value) : $value;
                    }, Traverse::merge(Map {
                        'field' => $field,
                        'title' => $fields[$field]
                    }, $options)));
                } else {
                    throw new InvalidValidationRuleException(sprintf('Error message for rule %s does not exist', $rule));
                }

                if (!call_user_func_array([$class, $rule], $arguments)) {
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
     * @param Map<string, mixed> $fields
     * @return $this
     */
    public static function makeFromShorthand(array $data = [], Map<string, mixed> $fields = {}): Validator {
        /** @type \Titon\Utility\Validator $obj */
        $obj = new static($data);

        foreach ($fields as $field => $options) {
            $title = $field;

            // Convert to array
            if (is_string($options)) {
                $options = Map {'rules' => $options};

            } else if (!is_array($options)) {
                continue;

            } else if (Traverse::isNumeric(array_keys($options))) {
                $options = ['rules' => $options];
            }

            // Prepare for parsing
            if (isset($options['title'])) {
                $title = $options['title'];
            }

            if (is_string($options['rules'])) {
                $options['rules'] = explode('|', $options['rules']);
            }

            $obj->addField($field, $title);

            foreach ($options['rules'] as $ruleOpts) {
                $shorthand = static::splitShorthand($ruleOpts);

                $obj->addRule($field, $shorthand['rule'], $shorthand['message'], $shorthand['options']);
            }
        }

        return $obj;
    }

    /**
     * Split a shorthand rule into multiple parts.
     *
     * @param string $shorthand
     * @return Map<string, mixed>
     */
    public static function splitShorthand(string $shorthand): Map<string, mixed> {
        $rule = null;
        $message = '';
        $opts = Vector {};

        // rule:o1,o2,o3
        // rule:o1,o2:The message here!
        if (strpos($shorthand, ':') !== false) {
            $parts = explode(':', $shorthand, 3);
            $rule = $parts[0];

            if (!empty($parts[1])) {
                $opts = $parts[1];

                if (strpos($opts, ',') !== false) {
                    $opts = new Vector(explode(',', $opts));
                } else {
                    $opts = new Vector([$opts]);
                }
            }

            if (!empty($parts[2])) {
                $message = $parts[2];
            }

        // rule
        } else {
            $rule = $shorthand;
        }

        return Map {
            'rule' => $rule,
            'message' => $message,
            'options' => $opts
        };
    }

}