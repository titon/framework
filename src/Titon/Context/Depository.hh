<?hh // strict
/**
 * @author Alex Phillips <exonintrendo@gmail.com>
 * Date: 2/5/15
 * Time: 4:45 PM
 */

namespace Titon\Context;

use Closure;
use ArrayAccess;
use ReflectionClass;
use ReflectionException;
use Titon\Context\Definition\CallableDefinition;
use Titon\Context\Definition\ClassDefinition;
use Titon\Context\Definition\Definition;

class Depository implements ArrayAccess
{
    /**
     * Hash of registered item definitions keyed by its alias or class name
     *
     * @var array
     */
    protected array $items = [];

    /**
     * Hash of registered, and already constructed, singletons keyd by its
     * alias or class name
     *
     * @var array
     */
    protected array $singletons = [];

    protected array $aliases = [];

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
            // @TODO: throw exception
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

    public function alias($alias, $key)
    {
        if (isset($this->aliases[$alias])) {
            // @TODO: throw exception
        }

        $this->aliases[$alias] = $key;
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
    public function make(string $alias, ...$arguments)
    {
        if (isset($this->aliases[$alias])) {
            return $this->make($this->aliases[$alias], ...$arguments);
        }

        if (isset($this->singletons[$alias])) {
            return $this->singletons[$alias];
        }

        if (array_key_exists($alias, $this->items)) {
            $definition = $this->items[$alias]['definition'];
            $retval = $definition;

            if ($definition instanceof CallableDefinition || $definition instanceof ClassDefinition) {
                $retval = $definition->create(...$arguments);
            }

            if (isset($this->items[$alias]['singleton']) && $this->items[$alias]['singleton'] === true) {
                unset($this->items[$alias]);
                $this->singletons[$alias] = $retval;
            }

            return $retval;
        }

        $definition = $this->build($alias);
        $this->items[$alias]['definition'] = $definition;

        return $definition->create(...$arguments);
    }

    protected function build($class, ...$parameters)
    {
        if (!class_exists($class)) {
            throw new ReflectionException("Class $class does not exist.");
        }

        $reflector = new ReflectionClass($class);
        if (!$reflector->isInstantiable()) {
            $message = "Target [$class] is not instantiable.";
            throw new ReflectionException($message);
        }

        $definition = Definition::factory($class, $class, $this);
        $constructor = $reflector->getConstructor();

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

                throw new ReflectionException(
                    sprintf('Unable to resolve a non-class dependency of [%s] for [%s]', $param, $class)
                );
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
    }
}