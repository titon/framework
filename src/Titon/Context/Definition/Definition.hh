<?hh // strict
/**
 * @author Alex Phillips <exonintrendo@gmail.com>
 * Date: 2/9/15
 * Time: 5:14 PM
 */

namespace Titon\Context\Definition;

use Titon\Context\Depository;

abstract class Definition
{
    protected string $alias;

    protected Depository $depository;

    protected array $arguments = [];

    public function __construct(string $alias, Depository $depository)
    {
        $this->alias = $alias;
        $this->depository = $depository;
    }

    public function with(...$arguments): this
    {
        foreach ($arguments as $argument) {
            $this->arguments[] = $argument;
        }

        return $this;
    }

    abstract public function create(...$arguments);

    public function resolveArguments(...$arguments)
    {
        if ($arguments) {
            $this->arguments = $arguments;
        }

        $resolvedArguments = [];

        foreach ($this->arguments as $argument) {
            if (is_string($argument) && (isset($this->depository[$argument]) || class_exists($argument))) {
                $resolvedArguments[] = $this->depository[$argument];
                continue;
            }

            $resolvedArguments[] = $argument;
        }

        return $resolvedArguments;
    }
}