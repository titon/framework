<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Console\InputDefinition;

use Titon\Console\InputDefinition;

/**
 * An InputDefinition is an object that designates the parameters accepted by
 * the user when executing commands.
 *
 * @package Titon\Console\InputDefinition
 */
abstract class AbstractInputDefinition<T> implements InputDefinition {

    /**
     * An alternate notation to specify the input as.
     *
     * @var string
     */
    protected string $alias = '';

    /**
     * The description of the input.
     *
     * @var string
     */
    protected string $description;

    /**
     * Flag if the `InputDefinition` has been assigned a value.
     *
     * @var bool
     */
    protected bool $exists = false;

    /**
     * The mode of the input to determine if it should be required by the user.
     *
     * @var int
     */
    protected int $mode = self::MODE_OPTIONAL;

    /**
     * The name and primary method to specify the input.
     *
     * @var string
     */
    protected string $name;

    /**
     * The value the user has given the input.
     *
     * @var T|null
     */
    protected ?T $value;

    /**
     * Cosntruct a new instance of an `InputDefinition`.
     *
     * @param string $name          The name of the parameter
     * @param string $description   The description used in displaying usage
     * @param int    $mode          The mode of the definition
     */
    public function __construct(string $name, string $description = '', int $mode = self::MODE_OPTIONAL) {
        $this->name = $name;
        $this->description = $description;
        $this->mode = $mode;
    }

    /**
     * Alias method for setting the alias of the `InputDefinition`.
     *
     * @param string $alias The input's alias
     *
     * @return $this
     */
    public function alias(string $alias): this {
        return $this->setAlias($alias);
    }

    /**
     * {@inheritdoc}
     */
    public function exists(): bool {
        return $this->exists;
    }

    /**
     * Retrieve the alias of the `InputDefinition`.
     *
     * @return string
     */
    public function getAlias(): string {
        return $this->alias;
    }

    /**
     * Retrieve the description of the `InputDefinition`.
     *
     * @return string
     */
    public function getDescription(): string {
        return $this->description;
    }

    /**
     * Retrieve the formatted name as it should be entered by the user.
     *
     * @param string $name The string name to format
     *
     * @return string
     */
    public function getFormattedName(string $name): string {
        if (strlen($name) === 1) {
            return "-$name";
        }

        return "--$name";
    }

    /**
     * Retrieve the mode of the `InputDefinition`.
     *
     * @return int
     */
    public function getMode(): int {
        return $this->mode;
    }

    /**
     * Retrieve the name of the `InputDefinition`.
     *
     * @return string
     */
    public function getName(): string {
        return $this->name;
    }

    /**
     * Retrieve the value as specified by the user for the `InputDefinition`. If
     * the user has not specified the value, the default value is returned.
     *
     * @return T|null
     */
    public function getValue(?T $default = null): ?T {
        if (!is_null($this->value)) {
            return $this->value;
        }

        return $default;
    }

    /**
     * Set the alias of the `InputDefinition`.
     *
     * @param string $alias The alternate name to specify
     *
     * @return string
     */
    public function setAlias(string $alias): this {
        if (strlen($alias) > strlen($this->name)) {
            $this->alias = $this->name;
            $this->name = $alias;
        } else {
            $this->alias = $alias;
        }

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function setExists(bool $exists): this {
        $this->exists = $exists;

        return $this;
    }

    /**
     * Set the value of the `Flag`.
     *
     * @param int $value The value given to the `Flag`
     *
     * @return $this
     */
    public function setValue(T $value): this {
        $this->value = $value;

        return $this;
    }
}