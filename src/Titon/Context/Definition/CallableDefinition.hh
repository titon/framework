<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Context\Definition;

use Closure;
use Titon\Context\Depository;

/**
 * A callable definition determines how a callable is created, including necessary
 * arguments passed into the function as well as methods to be called after
 * instantiation before returning the result for use.
 *
 * @package Titon\Context\Definition
 */
class CallableDefinition extends Definition implements DefinitionInterface
{
    /**
     * The Closure object defined
     *
     * @var Closure
     */
    protected Closure $callable;

    /**
     * {@inheritdoc}
     */
    public function __construct(string $alias, callable $concrete, Depository $depository)
    {
        parent::__construct($alias, $depository);

        $this->callable = $concrete;
    }

    /**
     * {@inheritdoc}
     */
    public function create(...$arguments)
    {
        return call_user_func_array(
            $this->callable,
            $this->resolveArguments(...$arguments)
        );
    }
}