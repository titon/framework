<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Context;

use Closure;
use ReflectionClass;
use ReflectionMethod;
use ReflectionFunction;
use ReflectionException;
use Titon\Context\Definition;
use Titon\Context\Definition\DefinitionFactory;
use Titon\Context\Exception\AlreadyRegisteredException;

/**
 * The depository serves as a dependency injector. After registering an object,
 * class, or callable with the depository, retrieving it will handle any necessary
 * dependency injection and reflection resolution before returning the object for
 * use.
 *
 * @package Titon\Context
 */
class Depository {

    /**
     * Hash of registered item definitions keyed by its alias or class name.
     *
     * @var \Titon\Context\ItemMap
     */
    protected ItemMap $items = Map {};

    /**
     * Hash of registered, and already constructed, singletons keyed by its
     * alias or class name.
     *
     * @var \Titon\Context\SingletonMap
     */
    protected SingletonMap $singletons = Map {};

    /**
     * Map of aliases to registered classes and keys.
     *
     * @var \Titon\Context\AliasMap
     */
    protected AliasMap $aliases = Map {};

    /**
     * Instantiate a new container object.
     */
    public function __construct() {
        $this->singleton('Titon\Context\Depository', $this);
    }

    /**
     * Register a new class, callable, or object in the container.
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
     * @throws \Titon\Context\Exception\AlreadyRegisteredException
     */
    public function register(string $key, mixed $concrete = null, bool $singleton = false): mixed {
        if ($this->isRegistered($key)) {
            throw new AlreadyRegisteredException("Key $key has already been registered");
        }

        if (!$concrete) {
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

        if (is_string($concrete) && $key !== $concrete && !is_callable($concrete)) {
            $this->aliases[$key] = $concrete;
            $key = $concrete;
        }

        // we need to build a definition
        $definition = DefinitionFactory::factory($key, $concrete, $this);

        $this->items[$key] = shape(
            'definition' => $definition,
            'singleton'  => $singleton
        );

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
     * @throws \Titon\Context\Exception\AlreadyRegisteredException
     */
    public function alias(string $alias, string $key): this {
        if ($this->aliases->contains($alias)) {
            throw new AlreadyRegisteredException("Alias $alias has already been mapped to {$this->aliases[$alias]}");
        }

        $this->aliases[$alias] = $key;

        return $this;
    }

    /**
     * Register a new singleton in the container.
     *
     * @param string $alias     The alias (container key) for the registered item
     * @param mixed $concrete   The class name, closure, object to register in
     *                          the container, or null to use the alias as the
     *                          class name
     *
     * @return object|$definition   Either the concrete (if an object is registered)
     *                              or the definition of the registered item
     */
    public function singleton(string $alias, mixed $concrete): mixed {
        return $this->register($alias, $concrete, true);
    }

    /**
     * Retrieve (and build if necessary) the registered item from the container.
     *
     * @param string $alias                 Key or alias of a registered item or a class
     *                                      name, callable, or Closure to construct
     * @param array<mixed> ...$arguments    Additional arguments to pass into the item at
     *                                      construction
     *
     * @return mixed    The resolved registered item or return value
     */
    public function make(string $alias, ...$arguments): mixed {
        if (is_string($alias) && $this->isRegistered($alias)) {
            return $this->getRegisteredItem($alias, ...$arguments);
        }

        if (class_exists($alias)) {
            $definition = $this->buildClass($alias);
        } else {
            $definition = $this->buildCallable($alias);
        }

        $this->items[$alias] = shape(
            'definition' => $definition,
            'singleton'  => false
        );

        return $definition->create(...$arguments);
    }

    /**
     * Immediately build a closure and run it.
     *
     * @param \Closure $callable
     * @param ...$arguments
     * @return mixed    The resolved registered item or return value
     */
    public function run(Closure $callable, ...$arguments): mixed {
        $definition = $this->buildCallable($callable);

        return $definition->create(...$arguments);
    }

    /**
     * Remove an alias or key from the depository's registry.
     *
     * @param string $key The key to remove
     *
     * @return $this Return the depository for fluent method chaining
     */
    public function remove(string $key): this {
        $this->singletons->remove($key);
        $this->items->remove($key);

        if ($this->aliases->contains($key)) {
            $this->singletons->remove($this->aliases[$key]);
            $this->items->remove($this->aliases[$key]);
            $this->aliases->remove($key);
        }

        return $this;
    }

    /**
     * Return whether or not an alias has been registered in the container
     *
     * @param string $alias Registered key or class name
     *
     * @return bool
     */
    public function isRegistered(string $alias): bool {
        if ($this->aliases->contains($alias)) {
            return true;
        }

        if ($this->singletons->contains($alias)) {
            return true;
        }

        if ($this->items->contains($alias)) {
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
    public function isSingleton(string $alias): bool {
        if ($this->aliases->contains($alias)) {
            return $this->isSingleton($this->aliases[$alias]);
        }

        if ($this->singletons->contains($alias) || ($this->items->contains($alias) && $this->items[$alias]['singleton'] === true)) {
            return true;
        }

        return false;
    }

    /**
     * This method will use reflection to build a definition of the callable to
     * be registered by the depository.
     *
     * @param string $alias
     *
     * @return Definition   The definition built from the callable
     */
    protected function buildCallable(mixed $alias): Definition {
        if (is_string($alias) && strpos($alias, '::') !== false) {
            $callable = explode('::', $alias);
        } else {
            $callable = $alias;

            if (!is_string($alias)) {
                $alias = 'Callable';
            }
        }

        $definition = DefinitionFactory::factory($alias, $callable, $this);

        if (is_array($callable)) {
            $reflector = new ReflectionMethod($callable[0], $callable[1]);
        } else {
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
     * This method will use reflection to build the class and inject any
     * necessary arguments for construction.
     *
     * @param string $class         The class name to reflect and construct
     * @param mixed ...$parameters  Parameters required for constructing the object
     *
     * @return Definition
     * @throws ReflectionException
     */
    protected function buildClass(string $class): Definition {
        $reflection = new ReflectionClass($class);

        if (!$reflection->isInstantiable()) {
            throw new ReflectionException("Target [$class] is not instantiable");
        }

        $definition = DefinitionFactory::factory($class, $class, $this);
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
     * Retrieve the created definition or stored instance from the depository
     * by key
     *
     * @param string $alias                 The key the item is stored under
     * @param array<mixed> ...$arguments    Arguments passed into creating the
     *                                      definition
     *
     * @return mixed
     */
    protected function getRegisteredItem(string $alias, ...$arguments): mixed {
        if ($this->aliases->contains($alias)) {
            return $this->make($this->aliases[$alias], ...$arguments);
        }

        if ($this->singletons->contains($alias)) {
            $retval = $this->singletons[$alias];
        } else {
            $definition = $this->items[$alias]['definition'];
            $retval = $definition;

            if ($definition instanceof Definition) {
                $retval = $definition->create(...$arguments);
            }

            if ($this->items->contains($alias) && $this->items[$alias]['singleton'] === true) {
                $this->items->remove($alias);
                $this->singletons[$alias] = $retval;
            }
        }

        return $retval;
    }

}
