<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Context\Definition;

use Closure;
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

    public static function factory(string $alias, mixed $concrete, Depository $depository)
    {
        if ($concrete instanceof Closure) {
            return new CallableDefinition($alias, $concrete, $depository);
        }

        if (is_string($concrete) && class_exists($concrete)) {
            return new ClassDefinition($alias, $concrete, $depository);
        }

        return $concrete;
    }

    public function with(...$arguments): this
    {
        foreach ($arguments as $argument) {
            $this->arguments[] = $argument;
        }

        return $this;
    }

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

    public function alias(string $alias): this
    {
        $this->depository->alias($alias, $this->alias);

        return $this;
    }
}