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
class CallableDefinition extends AbstractDefinition
{
    /**
     * The class the callable method exists in (if the callable is a method)
     *
     * @var string
     */
    protected string $class;

    /**
     * The method (if class is present) name or function name
     *
     * @var string
     */
    protected string $function;

    /**
     * {@inheritdoc}
     */
    public function __construct(string $key, mixed $callable, Depository $depository)
    {
        parent::__construct($key, $depository);

        if (is_array($callable)) {
            list($this->class, $this->function) = $callable;
        }
        else {
            $this->function = $callable;
        }
    }

    /**
     * {@inheritdoc}
     */
    public function create<T>(...$arguments): T
    {
        $arguments = $this->resolveArguments(...$arguments);

        if (!is_null($this->class)) {
            $class = $this->depository->make($this->class);
            return call_user_func_array(array($class, $this->function), $arguments);
        }

        return call_user_func_array($this->function, $arguments);
    }
}