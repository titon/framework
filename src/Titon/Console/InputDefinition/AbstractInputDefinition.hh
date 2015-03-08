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
abstract class AbstractInputDefinition implements InputDefinition {

    const int MODE_OPTIONAL = 0;
    const int MODE_REQUIRED = 1;

    /**
     * An alternate notation to specify the input as.
     *
     * @var string
     */
    protected string $alias = '';

    /**
     * The default value given to the input if not specified by the user.
     *
     * @var mixed
     */
    protected mixed $default;

    /**
     * The description of the input.
     *
     * @var string
     */
    protected string $description;

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
     * @var mixed
     */
    protected mixed $value;

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
     * Method to return if the `InputDefinition` was specified by the user.
     *
     * @return bool
     */
    public function exists(): bool {
        return !is_null($this->value);
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
     * Retrieve the default value of the `InputDefinition`.
     *
     * @return mixed
     */
    public function getDefault(): mixed {
        return $this->default;
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
     * @return mixed
     */
    public function getValue(): mixed {
        if (!is_null($this->value)) {
            return $this->value;
        }

        return $this->default;
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
}