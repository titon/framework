<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Context\Definition;

use Titon\Context\Definition;
use Titon\Context\Depository;

/**
 * A definition is an object that defines how an object or callable should be
 * created when being called or instantiated from the depository object.
 *
 * @package Titon\Context\Definition
 */
abstract class AbstractDefinition implements Definition
{
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
     * @var ArgumentList
     */
    protected ArgumentList $arguments = [];

    /**
     * Construct a new definition instance
     *
     * @param string     $key         The class name or key associated with
     *                                  the definition
     * @param Depository $depository    The container that the definition is
     *                                  contained in
     */
    public function __construct(string $key, Depository $depository)
    {
        $this->key = $key;
        $this->depository = $depository;
    }

    /**
     * Factory method to generate the necessary definition given the passed in
     * parameters
     *
     * @param string     $key           The key (class name or alias) for the
     *                                  definition
     * @param mixed      $concrete      The concrete (class, class name, or callable)
     *                                  for the definition to reference
     * @param Depository $depository    The depository object the definition is
     *                                  contained in
     *
     * @return ClosureAbstractDefinition|ClassAbstractDefinition|mixed The definition object for
     *                                                  fluent method chaining
     */
    public static function factory<T>(string $key, mixed $concrete, Depository $depository): T
    {
        if ($concrete instanceof Closure) {
            return new ClosureDefinition($key, $concrete, $depository);
        }

        if (is_string($concrete) && class_exists($concrete)) {
            return new ClassDefinition($key, $concrete, $depository);
        }

        if (is_string($concrete) && strpos($concrete, '::') !== false) {
            $concrete = explode('::', $concrete);
        }

        if (is_callable($concrete)) {
            return new CallableDefinition($key, $concrete, $depository);
        }

        return $concrete;
    }

    /**
     * Arguments passed into the constructor when creating the object from
     * within the container
     *
     * @param mixed ...$arguments   Arguments passed into the constructor
     *
     * @return $this    The definition for fluent method chaining
     */
    public function with(...$arguments): this
    {
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
     * @param mixed ...$arguments   The arguments to resolve
     *
     * @return Vector   The resolved arguments
     */
    public function resolveArguments(...$arguments): array<mixed>
    {
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
    public function alias(string $alias): this
    {
        $this->depository->alias($alias, $this->key);

        return $this;
    }
}