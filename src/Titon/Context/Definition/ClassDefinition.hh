<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Context\Definition;

use ReflectionClass;
use ReflectionMethod;
use Titon\Context\Depository;

/**
 * A class definition determines how a class is created, including necessary
 * arguments passed into the constructor as well as methods to be called after
 * instantiation before returning the object for use.
 *
 * @package Titon\Context\Definition
 */
class ClassDefinition extends AbstractDefinition
{
    /**
     * The name of the class defined
     *
     * @var string
     */
    protected string $class;

    /**
     * Data structure of methods and arguments to be called on the class after
     * instantiation
     *
     * @var array
     */
    protected MethodVector $methods = Vector{};

    /**
     * Construct a new class definition
     *
     * @param string     $key           The class name or key associated with
     * @param mixed      $class         The class to define
     * @param Depository $depository    The container that the definition is
     *                                  contained in
     */
    public function __construct(string $key, mixed $class, Depository $depository)
    {
        parent::__construct($key, $depository);

        $this->class = $class;
    }

    /**
     * {@inheritdoc}
     */
    public function create<T>(...$arguments): T
    {
        $reflection = new ReflectionClass($this->class);
        $object = $reflection->newInstanceArgs(
            $this->resolveArguments(...$arguments)
        );

        return $this->callMethods($object);
    }

    /**
     * Add a method to be called on the class after instantiating a new instance
     *
     * @param string $method        Name of th emethod
     * @param        ...$arguments  Arguments to pass into the method
     *
     * @return $this    The definition for fluent method chaining
     */
    public function call(string $method, ...$arguments): this
    {
        $this->methods[] = shape(
            'method'    => $method,
            'arguments' => $arguments,
        );

        return $this;
    }

    /**
     * After instantiating the instance, this method will call any necessary
     * methods on the instance with the specified arguments
     *
     * @param object $object    The object to call the methods on
     *
     * @return mixed    The new object instance
     */
    protected function callMethods<T>(T $object): T
    {
        foreach ($this->methods as $method) {
            $reflection = new ReflectionMethod($object, $method['method']);

            foreach ($method['arguments'] as $arg) {
                $args[] = (is_string($arg) && class_exists($arg)) ? $this->depository->make($arg) : $arg;
            }

            $reflection->invokeArgs($object, $args);
        }

        return $object;
    }
}