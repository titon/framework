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

    protected string $alias = '';

    protected mixed $default;

    protected string $description;

    protected int $mode = self::VALUE_OPTIONAL;

    protected string $name;

    protected mixed $value;

    public function __construct(string $name, string $description = '', int $mode = self::VALUE_OPTIONAL) {
        $this->name = $name;
        $this->description = $description;
        $this->mode = $mode;
    }

    public function alias(string $alias): this {
        return $this->setAlias($alias);
    }

    public function exists(): bool {
        if (is_null($this->value)) {
            return false;
        }

        return true;
    }

    public function getAlias(): string {
        return $this->alias;
    }

    public function getDefault(): mixed {
        return $this->default;
    }

    public function getDescription(): string {
        return $this->description;
    }

    public function getFormattedName(string $name): string {
        if (strlen($name) === 1) {
            return "-$name";
        }

        return "--$name";
    }

    public function getMode(): int {
        return $this->mode;
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