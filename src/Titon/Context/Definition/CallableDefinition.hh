<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Context\Definition;

use Titon\Context\Depository;

/**
 * A callable definition determines how a callable is created, including necessary
 * arguments passed into the function as well as methods to be called after
 * instantiation before returning the result for use.
 *
 * @package Titon\Context\Definition
 */
class CallableDefinition extends AbstractDefinition {

    /**
     * The class the callable method exists in (if the callable is a method).
     *
     * @var string
     */
    protected string $class;

    /**
     * The method (if class is present) name or function name.
     *
     * @var string
     */
    protected string $function;

    /**
     * {@inheritdoc}
     */
    public function __construct(string $key, string $class, string $function, Depository $depository) {
        parent::__construct($key, $depository);

        $this->class = $class;
        $this->function = $function;
    }

    /**
     * {@inheritdoc}
     */
    public function create<T>(/* HH_FIXME[4033]: variadic + strict */ ...$arguments): T {
        $arguments = $this->resolveArguments(...$arguments);

        if ($this->class) {
            // UNSAFE
            // Since inst_meth() requires literal strings and we are passing variables.
            $callable = inst_meth($this->depository->make($this->class), $this->function);

            return $callable(...$arguments);
        }

        // UNSAFE
        // Since fun() requires literal strings and we are passing variables.
        $callable = fun($this->function);

        return $callable(...$arguments);
    }

}
