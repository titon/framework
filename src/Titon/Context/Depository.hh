<?hh // strict
/**
 * @author Alex Phillips <exonintrendo@gmail.com>
 * Date: 2/5/15
 * Time: 4:45 PM
 */

namespace Titon\Context;

use Closure;
use ArrayAccess;
use Titon\Context\Definition\Definition;
use Titon\Context\Definition\CallableDefinition;
use Titon\Context\Definition\Factory;

class Depository implements ArrayAccess
{
    protected Factory $factory;

    protected Array $items = [];

    protected Array $singletons = [];

    public function __construct()
    {
        $this->factory = new Factory();
        $this->add('Titon\Context\Container', $this);
    }

    public function add(string $alias, $concrete = null, boolean $singleton = false): this
    {
        if (is_null($concrete)) {
            $concrete = $alias;
        }

        if (is_object($concrete) && !($concrete instanceof Closure)) {
            $this->singletons[$alias] = $concrete;
            return null;
        }

        // we need to build a definition
        $definition = $this->factory->create($alias, $concrete, $this);

        $this->items[$alias] = [
            'definition' => $definition,
            'singleton'  => $singleton,
        ];

        return $this;
    }

    public function singleton(string $alias, $concrete): this
    {
        $this->add($alias, $concrete, true);

        return $this;
    }

    public function make(string $alias, ...$arguments)
    {
        if (isset($this->singletons[$alias])) {
            return $this->singletons[$alias];
        }

        if (array_key_exists($alias, $this->items)) {
            return $this->resolveDefinition($alias, $arguments);
        }
    }

    protected function resolveDefinition(string $alias, ...$arguments)
    {
        $retval = $this->items[$alias]['definition'];

        if ($retval instanceof CallableDefinition || $retval instanceof ClassDefinition) {
            $retval = $retval->create($arguments);
        }

        // store as a singleton if needed
        if (isset($this->items[$alias]['singleton']) && $this->items[$alias]['singleton'] === true) {
            $this->singletons[$alias] = $retval;
        }

        return $retval;
    }

    public function offsetExists(mixed $key): bool
    {
        if (!is_null($this->singletons->get($key))) {
            return true;
        }
        if (!is_null($this->items->get($key))) {
            return true;
        }

        return false;
    }

    public function offsetGet($key): mixed
    {
        return $this->make($key);
    }

    public function offsetSet(mixed $key, $value): mixed
    {
        return $this->add($key, $value);
    }

    public function offsetUnset($key)
    {
        $this->singletons->remove($key);
        $this->items->remove($key);
    }
}