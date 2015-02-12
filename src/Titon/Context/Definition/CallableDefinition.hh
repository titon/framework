<?hh // strict
/**
 * @author Alex Phillips <exonintrendo@gmail.com>
 * Date: 2/9/15
 * Time: 5:21 PM
 */

namespace Titon\Context\Definition;

use Titon\Context\Depository;

class CallableDefinition extends Definition implements DefinitionInterface
{
    protected Closure $callable;

    public function __construct(string $alias, callable $concrete, Depository $depository)
    {
        parent::__construct($alias, $depository);

        $this->callable = $concrete;
    }

    public function create(...$arguments)
    {
        return call_user_func_array(
            $this->callable,
            $this->resolveArguments(...$arguments)
        );
    }
}