<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Context\Definition;

use Closure;
use Titon\Context\Definition;
use Titon\Context\Depository;

/**
 * A definition is an object that defines how an object or callable should be
 * created when being called or instantiated from the depository object.
 *
 * @package Titon\Context\Definition
 */
abstract class AbstractDefinition implements Definition {
    /**
     * The key for the definition
     *
     * @var string
     */
    protected string $key;

    /**
     * Despository object the definition is stored in
     *
     * @var Depository
     */
    protected Depository $depository;

    /**
     * Arguments used to pass into the object or Closure
     *
     * @var array<mixed>
     */
    protected array<mixed> $arguments = [];

    /**
     * Construct a new definition instance
     *
     * @param string     $key         The class name or key associated with
     *                                  the definition
     * @param Depository $depository    The container that the definition is
     *                                  contained in
     */
    public function __construct(string $key, Depository $depository) {
        $this->key = $key;
        $this->depository = $depository;
    }

    /**
     * Arguments passed into the constructor when creating the object from
     * within the container
     *
     * @param array<mixed> ...$arguments   Arguments passed into the constructor
     *
     * @return $this    The definition for fluent method chaining
     */
    public function with(...$arguments): this {
        foreach ($arguments as $argument) {
            $this->arguments[] = $argument;
        }

        return $this;
    }

    /**
     * Given arguments required for the definition, this method resolves them
     * by either creating them through reflection or the depository object
     * to pass them into the definition
     *
     * @param array<mixed> ...$arguments   The arguments to resolve
     *
     * @return array<mixed> The resolved arguments
     */
    public function resolveArguments(...$arguments): array<mixed> {
        if ($arguments) {
            $this->arguments = $arguments;
        }

        $resolvedArguments = [];

        foreach ($this->arguments as $argument) {
            if (is_string($argument) && ($this->depository->isRegistered($argument) || class_exists($argument))) {
                $resolvedArguments[] = $this->depository->make($argument);
                continue;
            }

            $resolvedArguments[] = $argument;
        }

        return $resolvedArguments;
    }

    /**
     * Method to link a definition to an alias inside the depository object
     *
     * @param string $alias The alias to associate to the definition
     *
     * @return $this    The definition for fluent method chaining
     */
    public function alias(string $alias): this {
        $this->depository->alias($alias, $this->key);

        return $this;
    }
}