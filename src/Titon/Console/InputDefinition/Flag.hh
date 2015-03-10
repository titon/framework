<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Console\InputDefinition;

/**
 * A `Flag` is a boolean parameter (denoted by an integer) specified by a user.
 *
 * @package Titon\Console\InputDefinition
 */
class Flag extends AbstractInputDefinition<int> {

    /**
     * The negative alias of the `Flag` (i.e., --no-foo for -foo). A negative
     * value is only available if a 'long' `Flag` name is available.
     *
     * @var string
     */
    protected string $negativeAlias = '';

    /**
     * Whether the flag is stackable or not (i.e., -fff is given a value of 3).
     *
     * @var bool
     */
    protected bool $stackable = false;

    /**
     * Construct a new `Flag` object
     *
     * @param string $name          The name the flag should be specified with
     * @param string $description   The description of the flag
     * @param int    $mode          The mode of the flag
     * @param bool   $stackable     Whether the flag is stackable or not
     */
    public function __construct(string $name, string $description = '', int $mode = self::MODE_OPTIONAL, bool $stackable = false) {
        parent::__construct($name, $description, $mode);

        if (strlen($name) > 1) {
            $this->negativeAlias = "no-$name";
        }

        $this->default = 0;
        $this->stackable = $stackable;
    }

    /**
     * Retrieve the negative alias of the `Flag` or null of none.
     *
     * @return string
     */
    public function getNegativeAlias(): string {
        return $this->negativeAlias;
    }

    /**
     * If the `Flag` is stackable, increase its value for each occurrence of the
     * flag.
     *
     * @return $this
     */
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

    /**
     * Retrieve whether the `Flag` is stackable or not.
     *
     * @return bool
     */
    public function isStackable(): bool {
        return $this->stackable;
    }

    /**
     * Set an alias for the `Flag`. If the 'name' given at construction is a short
     * name and the alias set is long, the 'alias' given here will serve as the
     * 'name' and the original name will be set to the 'alias'.
     *
     * @param string $alias The alias for the `Flag`
     *
     * @return $this
     */
    public function setAlias(string $alias): this {
        parent::setAlias($alias);

        if (strlen($this->getName()) > 1) {
            $this->negativeAlias = "no-{$this->getName()}";
        }

        return $this;
    }

    /**
     * Set whether the `Flag` is stackable or not.
     *
     * @param bool $stackable
     *
     * @return $this
     */
    public function setStackable(bool $stackable): this {
        $this->stackable = $stackable;

        return $this;
    }
}