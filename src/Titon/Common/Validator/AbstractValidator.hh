<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Common\Validator;

use Titon\Common\DataMap;
use Titon\Common\Exception\InvalidArgumentException;
use Titon\Common\Exception\InvalidValidationRuleException;
use Titon\Common\Validator;
use Titon\Common\Validator\ErrorMap;
use Titon\Common\Validator\FieldMap;
use Titon\Common\Validator\MessageMap;
use Titon\Common\Validator\Rule;
use Titon\Common\Validator\RuleContainer;
use Titon\Common\Validator\RuleOptionList;
use Titon\Utility\Str;
use \Indexish;
use \ReflectionClass;

/**
 * Defines shared functionality for validators.
 *
 * @package Titon\Common\Validator
 */
abstract class AbstractValidator implements Validator {

    /**
     * Data to validate against.
     *
     * @var \Titon\Common\DataMap
     */
    protected DataMap $_data = Map {};

    /**
     * Errors gathered during validation.
     *
     * @var \Titon\Common\Validator\ErrorMap
     */
    protected ErrorMap $_errors = Map {};

    /**
     * Mapping of fields and titles.
     *
     * @var \Titon\Common\Validator\FieldMap
     */
    protected FieldMap $_fields = Map {};

    /**
     * Fallback mapping of error messages.
     *
     * @var \Titon\Common\Validator\MessageMap
     */
    protected MessageMap $_messages = Map {};

    /**
     * Mapping of fields and validation rules.
     *
     * @var \Titon\Common\Validator\RuleContainer
     */
    protected RuleContainer $_rules = Map {};

    /**
     * Store the data to validate.
     *
     * @param \Titon\Common\DataMap $data
     */
    public function __construct(DataMap $data = Map {}) {
        $this->setData($data);
    }

    /**
     * {@inheritdoc}
     */
    public function addError(string $field, string $message): this {
        $this->_errors[$field] = $message;

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function addField(string $field, string $title, Map<string, RuleOptionList> $rules = Map {}): this {
        $this->_fields[$field] = $title;

        if (!$rules->isEmpty()) {
            foreach ($rules as $rule => $options) {
                $this->addRule($field, $rule, '', $options);
            }
        }

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function addMessages(MessageMap $messages): this {
        $this->_messages->setAll($messages);

        return $this;
    }

    /**
     * {@inheritdoc}
     *
     * @throws \Titon\Common\Exception\InvalidArgumentException
     */
    public function addRule(string $field, string $rule, string $message, RuleOptionList $options = Vector{}): this {
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
     * {@inheritdoc}
     */
    public function getData(): DataMap {
        return $this->_data;
    }

    /**
     * {@inheritdoc}
     */
    public function getErrors(): ErrorMap {
        return $this->_errors;
    }

    /**
     * {@inheritdoc}
     */
    public function getFields(): FieldMap {
        return $this->_fields;
    }

    /**
     * {@inheritdoc}
     */
    public function getMessages(): MessageMap {
        return $this->_messages;
    }

    /**
     * {@inheritdoc}
     */
    public function getRules(): RuleContainer {
        return $this->_rules;
    }

    /**
     * {@inheritdoc}
     */
    public function reset(): this {
        $this->_data->clear();
        $this->_errors->clear();

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function setData(DataMap $data): this {
        $this->_data = $data;

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function validate(): bool {
        if (!$this->_data) {
            return false;
        }

        $fields = $this->getFields();
        $fieldRules = $this->getRules();
        $messages = $this->getMessages();

        foreach ($this->getData() as $field => $value) {
            if (!$fieldRules->contains($field)) {
                continue;
            }

            $rules = $fieldRules[$field];

            foreach ($rules as $rule => $params) {
                $options = $params['options'];
                $arguments = $options->toArray();
                array_unshift($arguments, $value);

                // Use G11n if it is available
                if (class_exists('Titon\G11n\Utility\Validate')) {
                    $class = 'Titon\G11n\Utility\Validate';
                } else {
                    $class = 'Titon\Utility\Validate';
                }

                if (!call_user_func(class_meth($class, 'hasRule'), $rule)) {
                    throw new InvalidValidationRuleException(sprintf('Validation rule %s does not exist', $rule));
                }

                // Prepare messages
                $message = $params['message'];

                if (!$message && $messages->contains($rule)) {
                    $message = $messages[$rule];
                }

                if ($message) {
                    $params = Map {
                        'field' => $field,
                        'title' => $fields[$field]
                    };

                    $params->setAll($options->toMap()->map($value ==> ($value instanceof Indexish) ? implode(', ', $value) : $value));

                    $message = Str::insert($message, $params);
                } else {
                    throw new InvalidValidationRuleException(sprintf('Error message for rule %s does not exist', $rule));
                }

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
     * @param \Titon\Common\DataMap $data
     * @param Map<string, mixed> $fields
     * @return $this
     */
    public static function makeFromShorthand(DataMap $data = Map {}, Map<string, mixed> $fields = Map {}): Validator {
        $class = new ReflectionClass(static::class);

        /** @var \Titon\Common\Validator $obj */
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

            // Dereference for the type checker
            $rules = $options['rules'];

            if ($rules instanceof Vector) {
                foreach ($rules as $ruleOpts) {
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
     * @return \Titon\Common\Validator\Rule
     */
    public static function splitShorthand(string $shorthand): Rule {
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