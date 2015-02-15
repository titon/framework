<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Context;

use Closure;
use ArrayAccess;
use ReflectionClass;
use ReflectionMethod;
use ReflectionFunction;
use ReflectionException;
use Titon\Context\Definition\CallableDefinition;
use Titon\Context\Definition\ClosureDefinition;
use Titon\Context\Definition\ClassDefinition;
use Titon\Context\Definition\Definition;
use Titon\Context\Exception\AlreadyRegisteredException;

/**
 * The depository serves as a dependency injector. After registering an object,
 * class, or callable with the depository, retrieving it will handle any necessary
 * dependency injection and reflection resolution before returning the object for
 * use.
 *
 * @package Titon\Context
 */
class Depository implements ArrayAccess
{
    /**
     * Hash of registered item definitions keyed by its alias or class name
     *
     * @var array
     */
    protected array $items = [];

    /**
     * Hash of registered, and already constructed, singletons keyed by its
     * alias or class name
     *
     * @var array
     */
    protected array $singletons = [];

    /**
     * Map of aliases to registered classes and keys
     *
     * @var AliasMap
     */
    protected AliasMap $aliases = Map{};

    /**
     * Instantiate a new container object
     */
    public function __construct()
    {
        $this->register('Titon\Context\Depository', $this);
    }

    /**
     * Register a new class, callable, or object in the container
     *
     * @param string $key     The alias (container key) for the registered item
     * @param mixed $concrete   The class name, closure, object to register in
     *                          the container, or null to use the alias as the
     *                          class name
     * @param bool  $singleton  Whether or not the container should register the
     *                          concrete as a singleton or not (only if concrete
     *                          is class name or a closure)
     *
     * @return object|$definition   Either the concrete (if an object is registered)
     *                              or the definition of the registered item
     */
    public function register(string $key, ?mixed $concrete = null, boolean $singleton = false): mixed
    {
        if (isset($this[$key]) || isset($this->aliases[$key])) {
            throw new AlreadyRegisteredException("Key $key has already been registered");
        }

        if (is_null($concrete)) {
            $concrete = $key;
        }

        if (is_object($concrete) && !($concrete instanceof Closure)) {
            if ($key !== get_class($concrete)) {
                $this->aliases[$key] = get_class($concrete);
                $key = get_class($concrete);
            }

            $this->singletons[$key] = $concrete;

            return $concrete;
        }

        if (is_string($concrete) && $key !== $concrete) {
            $this->aliases[$key] = $concrete;
            $key = $concrete;
        }

        // we need to build a definition
        $definition = Definition::factory($key, $concrete, $this);

        $this->items[$key] = [
            'definition' => $definition,
            'singleton'  => $singleton,
        ];

        return $definition;
    }

    /**
     * Alias a string to map to a registered item in the container. This allows
     * you to call 'make' on an alias that maps to a more complex class name,
     * Closure, or singleton instance.
     *
     * @param $alias string The alias to register
     * @param $key string   The original class name or key registered
     *
     * @return $this Return the depository for fluent method chaining
     */
    public function alias($alias, $key): this
    {
        if (isset($this->aliases[$alias])) {
            throw new AlreadyRegisteredException("Alias $alias has already been mapped to {$this->aliases[$alias]}");
        }

        $this->aliases[$alias] = $key;

        return $this;
    }

    /**
     * Register a new singleton in the container
     *
     * @param string $alias     The alias (container key) for the registered item
     * @param mixed $concrete   The class name, closure, object to register in
     *                          the container, or null to use the alias as the
     *                          class name
     *
     * @return object|$definition   Either the concrete (if an object is registered)
     *                              or the definition of the registered item
     */
    public function singleton(string $alias, ?mixed $concrete): mixed
    {
        return $this->register($alias, $concrete, true);
    }

    /**
     * Retrieve (and build if necessary) the registered item from the container
     *
     * @param string $alias         The alias that the item was registered as
     * @param mixed ...$arguments   Additional arguments to pass into the object
     *                              during
     *
     * @return mixed
     */
    public function make($alias, ...$arguments)
    {
        if ($alias instanceof Closure || is_callable($alias)) {
            $definition = $this->buildCallable($alias);

            return $definition->create(...$arguments);
        }

        if (isset($this->aliases[$alias])) {
            return $this->make($this->aliases[$alias], ...$arguments);
        }

        if (isset($this->singletons[$alias])) {
            return $this->singletons[$alias];
        }

        if (array_key_exists($alias, $this->items)) {
            $definition = $this->items[$alias]['definition'];
            $retval = $definition;

            if ($definition instanceof Definition) {
                $retval = $definition->create(...$arguments);
            }

            if (isset($this->items[$alias]['singleton']) && $this->items[$alias]['singleton'] === true) {
                unset($this->items[$alias]);
                $this->singletons[$alias] = $retval;
            }

            return $retval;
        }

        if (class_exists($alias)) {
            $definition = $this->buildClass($alias);
            $this->items[$alias]['definition'] = $definition;
        }
        else {
            $definition = $this->buildCallable($alias);
            $this->items[$alias]['definition'] = $definition;
        }

        return $definition->create(...$arguments);
    }

    /**
     * This method will use reflection to build the class and inject any
     * necessary arguments for construction.
     *
     * @param string $class         The class name to reflect and construct
     * @param mixed ...$parameters  Parameters required for constructing the object
     *
     * @return ClosureDefinition|CallableDefinition|ClassDefinition|Definition\mixed
     * @throws ReflectionException
     */
    protected function buildClass(string $class): Definition
    {
        if (!class_exists($class)) {
            throw new ReflectionException("Class $class does not exist.");
        }

        $reflection = new ReflectionClass($class);
        if (!$reflection->isInstantiable()) {
            $message = "Target [$class] is not instantiable.";
            throw new ReflectionException($message);
        }

        $definition = Definition::factory($class, $class, $this);
        $constructor = $reflection->getConstructor();

        if (is_null($constructor)) {
            return $definition;
        }

        foreach ($constructor->getParameters() as $param) {
            $dependency = $param->getClass();

            if (is_null($dependency)) {
                if ($param->isDefaultValueAvailable()) {
                    $definition->with($param->getDefaultValue());
                    continue;
                }

                throw new ReflectionException("Cannot to resolve dependency of $param for $class");
            }

            $definition->with($dependency->getName());
        }

        return $definition;
    }

    /**
     * This method will use reflection to build a definition of the callable to
     * be registered by the depository.
     *
     * @param string $alias
     */
    protected function buildCallable($alias): Definition
    {
        if (is_string($alias) && strpos($alias, '::') !== false) {
            $callable = explode('::', $alias);
        }
        else {
            $callable = $alias;

            if (!is_string($alias)) {
                $alias = 'Callable';
            }
        }

        $definition = Definition::factory($alias, $callable, $this);

        if (is_array($callable)) {
            $reflector = new ReflectionMethod($callable[0], $callable[1]);
        }
        else {
            $reflector = new ReflectionFunction($callable);
        }

        foreach ($reflector->getParameters() as $param) {
            $dependency = $param->getClass();

            if (is_null($dependency)) {
                if ($param->isDefaultValueAvailable()) {
                    $definition->with($param->getDefaultValue());
                    continue;
                }

                throw new ReflectionException("Cannot to resolve dependency of $param for $alias");
            }

            $definition->with($dependency->getName());
        }

        return $definition;
    }

    /**
     * Return whether or not an alias has been registered in the container
     *
     * @param string $alias Registered key or class name
     *
     * @return bool
     */
    public function isRegistered(string $alias): bool
    {
        if (isset($this->singletons[$alias])) {
            return true;
        }

        if (isset($this->items[$alias])) {
            return true;
        }

        return false;
    }

    /**
     * Return whether or not an alias has been registered as a singleton in
     * the container
     *
     * @param string $alias Registered key or class name
     *
     * @return bool
     */
    public function isSingleton(string $alias)
    {
        if (isset($this->singletons[$alias]) || (isset($this->items[$alias]) && $this->items[$alias]['singleton'] === true)) {
            return true;
        }

        return false;
    }

    /**
     * {@inheritdoc}
     */
    public function offsetExists(mixed $key): bool
    {
        return $this->isRegistered($key);
    }

    /**
     * {@inheritdoc}
     */
    public function offsetGet(mixed $key): mixed
    {
        return $this->make($key);
    }

    /**
     * {@inheritdoc}
     */
    public function offsetSet(mixed $key, $value): mixed
    {
        return $this->register($key, $value);
    }

    /**
     * {@inheritdoc}
     */
    public function offsetUnset(mixed $key)
    {
        unset($this->singletons[$key]);
        unset($this->items[$key]);

        if (isset($this->aliases[$key])) {
            unset($this->singletons[$this->aliases[$key]]);
            unset($this->items[$this->aliases[$key]]);
            unset($this->aliases[$key]);
        }
    }
}