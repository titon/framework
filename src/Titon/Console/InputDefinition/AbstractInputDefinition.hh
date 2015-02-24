<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Console\InputDefinition;

use Titon\Console\InputDefinition;

abstract class AbstractInputDefinition implements InputDefinition {

    const VALUE_OPTIONAL = 0;
    const VALUE_REQUIRED = 1;

    protected string $name;

    protected string $description;

    protected int $mode = self::VALUE_OPTIONAL;

    protected mixed $value;

    protected mixed $default;

    public function __construct(string $name, string $description, int $mode = self::VALUE_OPTIONAL) {
        $this->name = $name;
        $this->description = $description;
        $this->mode = $mode;
    }

    public function exists(): bool {
        if (is_null($this->value)) {
            return false;
        }

        return true;
    }

    public function getDefault(): mixed {
        return $this->default;
    }

    public function getDescription(): string {
        return $this->description;
    }

    public function getName(): string {
        return $this->name;
    }

    public function getValue(): mixed {
        if (!is_null($this->value)) {
            return $this->value;
        }

        return $this->default;
    }
}