<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Utility;

use Titon\Utility\Exception\InvalidArgumentException;
use Titon\Utility\Exception\InvalidValidationRuleException;
use \ReflectionClass;

newtype FieldRule = shape(
    'rule' => string,
    'message' => string,
    'options' => Vector<mixed>
);

/**
 * The Validator allows for quick validation against a defined set of rules and fields.
 *
 * @package Titon\Utility
 */
class Validator {

    /**
     * Data to validate against.
     *
     * @type Map<string, mixed>
     */
    protected Map<string, mixed> $_data = Map {};

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
     * @type Map<string, Map<string, FieldRule>>
     */
    protected Map<string, Map<string, FieldRule>> $_rules = Map {};

    /**
     * Store the data to validate.
     *
     * @param Map<string, mixed> $data
     */
    public function __construct(Map<string, mixed> $data = Map {}) {
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
     * @param Map<string, mixed> $rules
     * @return $this
     */
    public function addField(string $field, string $title, Map<string, Vector<mixed>> $rules = Map {}): this {
        $this->_fields[$field] = $title;

        /**
         * 0 => rule
         * rule => [opt, ...]
         */
        if (!$rules->isEmpty()) {
            foreach ($rules as $rule => $options) {
                $this->addRule($field, $rule, '', $options);
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
     * @param Vector<mixed> $options
     * @return $this
     * @throws \Titon\Utility\Exception\InvalidArgumentException
     */
    public function addRule(string $field, string $rule, string $message, Vector<mixed> $options = Vector{}): this {
        if (!$this->_fields->contains($field)) {
            throw new InvalidArgumentException(sprintf('Field %s does not exist', $field));
        }

        if ($this->_messages->contains($rule)) {
            $message = $message ?: $this->_messages[$rule];
        } else {
            $this->_messages[$rule] = $message;
        }

        if (!$this->_rules->contains($field)) {
            $this->_rules[$field] = Map {};
        }

        $this->_rules[$field][$rule] = shape(
            'rule' => $rule,
            'message' => $message,
            'options' => $options
        );

        return $this;
    }

    /**
     * Return the currently set data.
     *
     * @return Map<string, mixed>
     */
    public function getData(): Map<string, mixed> {
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
     * @return Map<string, Map<string, FieldRule>>
     */
    public function getRules(): Map<string, Map<string, FieldRule>> {
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
     * @param Map<string, mixed> $data
     * @return $this
     */
    public function setData(Map<string, mixed> $data): this {
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
            if (!$this->_rules->contains($field)) {
                continue;
            }

            $rules = $this->_rules[$field];


            foreach ($rules as $rule => $params) {
                $options = $params['options'];
                $arguments = $options->toVector(); // Clone another vector or else message params are out of order
                array_unshift($arguments, $value);

                // Use G11n if it is available
                // @codeCoverageIgnoreStart
                if (class_exists('Titon\G11n\Utility\Validate')) {
                    $class = 'Titon\G11n\Utility\Validate';
                } else {
                    $class = 'Titon\Utility\Validate';
                }
                // @codeCoverageIgnoreEnd

                // UNSAFE
                if (!call_user_func(class_meth($class, 'hasRule'), $rule)) {
                    throw new InvalidValidationRuleException(sprintf('Validation rule %s does not exist', $rule));
                }

                // Prepare messages
                $message = $params['message'];

                if (!$message && $messages->contains($rule)) {
                    $message = $messages[$rule];
                }

                if ($message) {
                    $params = Col::merge(Map {
                        'field' => $field,
                        'title' => $fields[$field]
                    }, $options->toMap())->map(function($value) {
                        return ($value instanceof Vector) ? implode(', ', $value) : $value;
                    });

                    $message = Str::insert($message, $params);
                } else {
                    throw new InvalidValidationRuleException(sprintf('Error message for rule %s does not exist', $rule));
                }

                // UNSAFE
                if (!call_user_func_array(class_meth($class, $rule), $arguments)) {
                    $this->addError($field, $message);
                    break;
                }
            }
        }

        return (count($this->_errors) === 0);
    }

    /**
     * Create a validator instance from a set of shorthand or expanded rule sets.
     *
     * @param Map<string, mixed> $data
     * @param Map<string, mixed> $fields
     * @return $this
     */
    public static function makeFromShorthand(Map<string, mixed> $data = Map {}, Map<string, mixed> $fields = Map {}): Validator {
        $class = new ReflectionClass(static::class);

        /** @type \Titon\Utility\Validator $obj */
        $obj = $class->newInstanceArgs([$data]);

        foreach ($fields as $field => $options) {
            $title = $field;

            // A string of rule(s)
            if (is_string($options)) {
                $options = Map {'rules' => $options};

            // List of rules
            } else if ($options instanceof Vector) {
                $options = Map {'rules' => $options};

            // Ignore anything else not a map
            } else if (!$options instanceof Map) {
                continue;
            }

            // Prepare for parsing
            if ($options->contains('title')) {
                $title = $options['title'];
            }

            if (is_string($options['rules'])) {
                $options['rules'] = new Vector(explode('|', $options['rules']));
            }

            $obj->addField($field, (string) $title);

            if ($options['rules'] instanceof Vector) {
                foreach ($options['rules'] as $ruleOpts) {
                    $shorthand = static::splitShorthand($ruleOpts);

                    $obj->addRule($field, $shorthand['rule'], $shorthand['message'], $shorthand['options']);
                }
            }
        }

        return $obj;
    }

    /**
     * Split a shorthand rule into multiple parts.
     *
     * @param string $shorthand
     * @return FieldRule
     */
    public static function splitShorthand(string $shorthand): FieldRule {
        $rule = '';
        $message = '';
        $opts = Vector {};

        // rule:o1,o2,o3
        // rule:o1,o2:The message here!
        if (strpos($shorthand, ':') !== false) {
            foreach (explode(':', $shorthand, 3) as $index => $part) {
                if ($index == 0) {
                    $rule = $part;

                } else if ($index == 1) {
                    if (strpos($part, ',') !== false) {
                        $opts = new Vector(explode(',', $part));
                    } else if ($part) {
                        $opts = new Vector([$part]);
                    }

                } else if ($index == 2) {
                    $message = $part;
                }
            }

        // rule
        } else {
            $rule = $shorthand;
        }

        return shape(
            'rule' => $rule,
            'message' => $message,
            'options' => $opts
        );
    }

}