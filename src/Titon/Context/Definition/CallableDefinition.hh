<?hh // strict
/**
 * @author Alex Phillips <exonintrendo@gmail.com>
 * Date: 2/9/15
 * Time: 5:21 PM
 */

namespace Titon\Context\Definition;

use Closure;
use Titon\Context\Depository;

class CallableDefinition extends Definition
{
    protected Closure $callable;

    public function __construct(string $alias, Closure $concrete, Depository $canister)
    {
        parent::__construct($alias, $canister);

        $this->callable = $concrete;
    }

    public function create(...$arguments)
    {
        if ($arguments) {
            $this->arguments = [];
            $this->with($arguments);
        }

        return call_user_func_array($this->callable, $this->resolveArguments());
    }
}