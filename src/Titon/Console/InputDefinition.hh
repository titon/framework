<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Console;

/**
 * An `InputDefinition` defines the name and type of input that may be accepted
 * by the user.
 *
 * @package Titon\Console
 */
interface InputDefinition {

    const int MODE_OPTIONAL = 0;
    const int MODE_REQUIRED = 1;

    /**
     * Returns if the `InputDefinition` has been assigned a value by the parser.
     *
     * @return bool
     */
    public function exists(): bool;

    /**
     * Retrieve the alias of the `InputDefinition`.
     *
     * @return string
     */
    public function getAlias(): string;

    /**
     * Retrieve the description of the `InputDefinition`.
     *
     * @return string
     */
    public function getDescription(): string;

    /**
     * Retrieve the formatted name suitable for output in a help screen or
     * documentation.
     *
     * @return string
     */
    public function getFormattedName(string $name): string;

    /**
     * Retrieve the mode of the `InputDefinition`.
     *
     * @return string
     */
    public function getMode(): int;

    /**
     * Retrieve the name of the `InputDefinition`.
     *
     * @return string
     */
    public function getName(): string;

    /**
     * Retrieve the value of the `InputDefinition` as specified by the user.
     *
     * @return mixed
     */
    public function getValue(): mixed;

    /**
     * Set if the `InputDefinition` has been assigned a value.
     *
     * @param bool $exists  If a value exists
     *
     * @return $this
     */
    public function setExists(bool $exists): this;
}