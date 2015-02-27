<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Console\InputDefinition;

class Option extends AbstractInputDefinition {

    public function getValue(mixed $default = null): mixed {
        if (!is_null($this->value)) {
            return $this->value;
        }

        return $default;
    }

    public function setDefault(string $default): this {
        $this->default = $default;

        return $this;
    }

    public function setValue(string $value): this {
        $this->value = $value;

        return $this;
    }
}