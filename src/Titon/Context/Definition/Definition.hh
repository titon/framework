<?hh // strict
/**
 * @author Alex Phillips <exonintrendo@gmail.com>
 * Date: 2/9/15
 * Time: 5:14 PM
 */

namespace Titon\Context\Definition;

use Titon\Context\Depository;

abstract class Definition implements DefinitionInterface
{
    protected string $alias;
    protected Canister $canister;
    protected Vector<mixed> $arguments = Vector{};

    public function __construct(string $alias, Depository $canister) {}

    public function with(...$arguments): this
    {
        foreach ($arguments as $argument) {
            $this->arguments[] = $argument;
        }

        return $this;
    }

    abstract public function create(...$arguments);

    public function resolveArguments()
    {
        $resolvedArguments = [];

        foreach ($this->arguments as $argument) {
            if (is_string($argument)) {
                $resolvedArguments[] = $this->canister->make($argument);
                continue;
            }

            $resolvedArguments[] = $argument;
        }

        return $resolvedArguments;
    }
}