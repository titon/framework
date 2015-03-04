<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Console\InputDefinition;

/**
 * A `Flag` is a value parameter specified by a user.
 *
 * @package Titon\Console\InputDefinition
 */
class Option extends AbstractInputDefinition {

    /**
     * Set the default value for the `Option` if no value is give.
     *
     * @param int $default  The default value
     *
     * @return $this
     */
    public function setDefault(string $default): this {
        $this->default = $default;

        return $this;
    }

    /**
     * Set the value of the `Option`.
     *
     * @param string $value The value given to the `Option`
     *
     * @return $this
     */
    public function setValue(string $value): this {
        $this->value = $value;

        return $this;
    }
}