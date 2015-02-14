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
 * A closure definition determines how a closure is created, including necessary
 * arguments passed into the function as well as methods to be called after
 * instantiation before returning the result for use.
 *
 * @package Titon\Context\Definition
 */
class ClosureDefinition extends Definition implements DefinitionInterface
{
    /**
     * The Closure object defined
     *
     * @var Closure
     */
    protected Closure $closure;

    /**
     * {@inheritdoc}
     */
    public function __construct(string $alias, Closure $closure, Depository $depository)
    {
        parent::__construct($alias, $depository);

        $this->closure = $closure;
    }

    /**
     * {@inheritdoc}
     */
    public function create(...$arguments)
    {
        $arguments = $this->resolveArguments(...$arguments);

        return call_user_func_array(
            $this->closure,
            $this->resolveArguments(...$arguments)
        );
    }
}