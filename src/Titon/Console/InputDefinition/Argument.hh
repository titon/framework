<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Console\InputDefinition;

/**
 * An `Argument` is a parameter specified by the user that does not use any
 * notation (i.e., --foo, -f).
 *
 * @package Titon\Console\InputDefinition
 */
class Argument extends AbstractInputDefinition {

    /**
     * {@inheritdoc}
     */
    <<__Override>>
    public function getFormattedName(string $name): string {
        return $name;
    }

    /**
     * Set the value of the `Argument`.
     *
     * @param string $value The value given to the `Argument`
     *
     * @return $this
     */
    public function setValue(string $value): this {
        $this->value = $value;

        return $this;
    }
}