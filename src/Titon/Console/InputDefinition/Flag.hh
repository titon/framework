<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Console\InputDefinition;

class Flag extends AbstractInputDefinition {

    protected ?string $alias;

    protected bool $stackable = false;

    public function __construct(string $name, string $description, int $mode = self::VALUE_OPTIONAL, bool $stackable = false) {
        parent::__construct($name, $description, $mode);

        $this->default = 0;
        $this->stackable = $stackable;
    }

    public function getValue(): int {
        if (!is_null($this->value)) {
            return (int)$this->value;
        }

        return (int)$this->default;
    }

    public function increaseValue(): this {
        if ($this->stackable) {
            if (is_null($this->value)) {
                $this->value = 1;
            } else {
                invariant($this->value instanceof \int, 'Must be an annotation.');

                $this->value++;
            }
        }

        return $this;
    }

    public function isStackable(): bool {
        return $this->stackable;
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

    public function setValue(int $value): this {
        $this->value = $value;

        return $this;
    }
}