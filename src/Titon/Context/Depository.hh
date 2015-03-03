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
use Titon\Context\Definition\ObjectDefinition;
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
     * Vector collection of all Service Provider objects registered with the
     * Depository.
     *
     * @var \Titon\Context\ProviderList
     */
    protected ProviderList $providers = Vector {};

    /**
     * Map of aliases to registered classes and keys.
     *
     * @var \Titon\Context\AliasMap
     */
    protected AliasMap $aliases = Map {};

    /**
     * Instantiate a persistent container object.
     *
     * @var \Titon\Context\Depository
     */
    protected static ?Depository $instance;

    /**
     * Instantiate a new container object
     */
    public function __construct() {
        $this->singleton('Titon\Context\Depository', $this);
    }

    /**
     * Register a new Service Provider object in the container.
     *
     * @param mixed $serviceProvider
     * @return $this
     */
    public function addServiceProvider(mixed $serviceProvider): this {
        if (!($serviceProvider instanceof ServiceProvider)) {
            $serviceProvider = $this->make((string) $serviceProvider);
        }

        // Type checker thinks we are `mixed` here instead of `ServiceProvider`. Reset it.
        invariant($serviceProvider instanceof ServiceProvider, 'Must be a ServiceProvider.');

        $serviceProvider->setDepository($this);

        // Initialize the provider immediately if it provides nothing
        if ($serviceProvider->getProvides()->isEmpty()) {
            $serviceProvider->initialize();
        }

        $this->providers[] = $serviceProvider;

        return $this;
    }

    /**
     * Alias a string to map to a registered item in the container. This allows
     * you to call 'make' on an alias that maps to a more complex class name,
     * Closure, or singleton instance.
     *
     * @param $alias string The alias to register
     * @param $key string   The original class name or key registered
     * @return $this
     */
    public function alias(string $alias, string $key): this {
        if ($this->aliases->contains($alias)) {
            throw new AlreadyRegisteredException(sprintf('Alias %s has already been mapped to %s', $alias, $this->aliases[$alias]));
        }

        $this->aliases[$alias] = $key;

        return $this;
    }

    /**
     * Clear all registered items, singletons, and aliases in the Depository.
     *
     * @return $this
     */
    public function clear(): this {
        $this->aliases->clear();
        $this->singletons->clear();
        $this->items->clear();

        return $this;
    }

    /**
     * Retrieve the Depository singleton
     *
     * @return \Titon\Context\Depository
     */
    public static function getInstance(): Depository {
        if (is_null(self::$instance)) {
            self::$instance = new Depository();
        }

        return self::$instance;
    }

    /**
     * Determines if a Service Provider `provides` the given class name and,
     * if so, initializes the Service Provider for the Depository to resolve
     * it.
     *
     * @param string $key   The class name to check.
     * @return bool
     */
    public function isInServiceProvider(string $className): bool {
        foreach ($this->providers as $provider) {
            if ($provider->provides($className)) {
                $provider->initialize();

                return true;
            }
        }

        return false;
    }

    /**
     * Return whether or not an alias has been registered in the container.
     *
     * @param string $alias Registered key or class name
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
     * the container.
     *
     * @param string $alias Registered key or class name
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
     * Retrieve (and build if necessary) the registered item from the container.
     *
     * @param string $alias                 Key or alias of a registered item or a class
     *                                      name, callable, or Closure to construct
     * @param array<mixed> ...$arguments    Additional arguments to pass into the item at
     *                                      construction
     * @return mixed    The resolved registered item or return value
     */
    public function make(mixed $alias, /* HH_FIXME[4033]: variadic + strict */ ...$arguments): mixed {
        $definition = null;

        if (is_string($alias)) {
            if ($this->isRegistered($alias)) {
                return $this->getRegisteredItem($alias, ...$arguments);

            } else if ($this->isInServiceProvider($alias)) {
                return $this->make($alias, ...$arguments);

            } else if (class_exists($alias)) {
                $definition = $this->buildClass($alias, ...$arguments);
            }
        }

        if (!$definition) {
            $definition = $this->buildCallable($alias);
        }

        return $definition->create(...$arguments);
    }

    /**
     * Change an existing item to be used as a singleton or, if alias doesn't
     * exist, register passed in alias and existing concrete as singleton.
     *
     * @param string $alias     The key or alias to change to a singleton
     * @param mixed  $concrete  Class name, Closure, or object to register if
     *                          no item is currently registered as the alias.
     *                          If an item is already registered, the existing
     *                          item will be used and this concrete will be
     *                          ignored.
     * @return mixed The concrete object
     */
    public function makeSingleton(string $alias, mixed $concrete = null): mixed {
        if ($this->singletons->contains($alias)) {
            return $this->singletons[$alias];
        }

        if ($this->aliases->contains($alias)) {
            return $this->makeSingleton($this->aliases[$alias]);
        }

        // Item exists, mark as singleton
        if ($this->items->contains($alias)) {
            $this->items[$alias]['singleton'] = true;

        // Item doesn't exist, register it
        } else {
            $this->singleton($alias, $concrete);
        }

        return $this->make($alias);
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
     * @return \Titon\Context\Definition
     */
    public function register(string $key, mixed $concrete = null, bool $singleton = false): Definition {
        if ($this->isRegistered($key)) {
            throw new AlreadyRegisteredException(sprintf('Key %s has already been registered', $key));
        }

        if (!$concrete) {
            $concrete = $key;
        }

        // If an instantiated object, add directly to singletons
        if (is_object($concrete) && !($concrete instanceof Closure)) {
            $className = get_class($concrete);
            $singleton = true;

            if ($key !== $className) {
                $this->aliases[$key] = $className;
                $key = $className;
            }
        }

        // If not a callable, add aliasing
        if (is_string($concrete) && $key !== $concrete && !is_callable($concrete)) {
            $this->aliases[$key] = $concrete;
            $key = $concrete;
        }

        // Create the definition
        $definition = DefinitionFactory::factory($key, $concrete, $this);

        $this->items[$key] = shape(
            'definition' => $definition,
            'singleton'  => $singleton
        );

        return $definition;
    }

    /**
     * Remove an alias or key from the depository's registry.
     *
     * @param string $key The key to remove
     * @return $this
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
     * Register a new singleton in the container.
     *
     * @param string $alias     The alias (container key) for the registered item
     * @param mixed $concrete   The class name, Closure, or object to register in
     *                          the container, or null to use the alias as the
     *                          class name
     * @return \Titon\Context\Definition
     */
    public function singleton(string $alias, mixed $concrete = null): Definition {
        return $this->register($alias, $concrete, true);
    }

    /**
     * This method will use reflection to build the class and inject any
     * necessary arguments for construction.
     *
     * @param string $class         The class name to reflect and construct
     * @param mixed ...$arguments   Arguments required for constructing the object
     * @return \Titon\Context\Definition
     * @throws ReflectionException
     */
    protected function buildClass(string $class, /* HH_FIXME[4033]: variadic + strict */ ...$arguments): Definition {
        $reflection = new ReflectionClass($class);

        if (!$reflection->isInstantiable()) {
            throw new ReflectionException(sprintf('Target %s is not instantiable', $class));
        }

        $definition = DefinitionFactory::factory($class, $class, $this);
        $constructor = $reflection->getConstructor();

        if (is_null($constructor)) {
            return $definition;
        }

        if (count($arguments) > 0) {
            foreach ($arguments as $arg) {
                $definition->with($arg);
            }
        } else {
            foreach ($constructor->getParameters() as $param) {
                $dependency = $param->getClass();

                if (is_null($dependency)) {
                    if ($param->isDefaultValueAvailable()) {
                        $definition->with($param->getDefaultValue());
                        continue;
                    }

                    throw new ReflectionException(sprintf('Cannot to resolve dependency of %s for %s', $param, $class));
                }

                $definition->with($dependency->getName());
            }
        }

        return $definition;
    }

    /**
     * This method will use reflection to build a definition of the callable to
     * be registered by the depository.
     *
     * @param string $alias
     * @return \Titon\Context\Definition
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

                throw new ReflectionException(sprintf('Cannot to resolve dependency of %s for %s', $param, $alias));
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
    protected function getRegisteredItem(string $alias, /* HH_FIXME[4033]: variadic + strict */ ...$arguments): mixed {
        if ($this->aliases->contains($alias)) {
            return $this->make($this->aliases[$alias], ...$arguments);
        }

        if ($this->singletons->contains($alias)) {
            $retval = $this->singletons[$alias];

        } else {
            $definition = $this->items[$alias]['definition'];
            $retval = $definition;

            if ($definition instanceof ObjectDefinition) {
                $retval = $definition->get();

            } else if ($definition instanceof Definition) {
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
