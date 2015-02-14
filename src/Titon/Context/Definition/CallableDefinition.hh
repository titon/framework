<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
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