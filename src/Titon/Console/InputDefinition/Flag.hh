<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Console\InputDefinition;

class Flag extends AbstractInputDefinition {

    protected ?string $negativeAlias;

    protected bool $stackable = false;

    public function __construct(string $name, string $description = '', int $mode = self::VALUE_OPTIONAL, bool $stackable = false) {
        parent::__construct($name, $description, $mode);

        if (strlen($name) > 1) {
            $this->negativeAlias = "no-$name";
        }

        $this->default = 0;
        $this->stackable = $stackable;
    }

    public function getNegativeAlias(): ?string {
        return $this->negativeAlias;
    }

    public function increaseValue(): this {
        if ($this->stackable) {
            if (is_null($this->value)) {
                $this->value = 1;
            } else {
                invariant(is_int($this->value), 'Must be an integer.');

                $this->value++;
            }
        }

        return $this;
    }

    public function isStackable(): bool {
        return $this->stackable;
    }

    public function setAlias(string $alias): this {
        parent::setAlias($alias);

        if (strlen($this->getName()) > 1) {
            $this->negativeAlias = "no-{$this->getName()}";
        }

        return $this;
    }

    public function setStackable(bool $stackable): this {
        $this->stackable = $stackable;

        return $this;
    }

    public function setValue(int $value): this {
        $this->value = $value;

        return $this;
    }
}