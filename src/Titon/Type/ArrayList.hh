<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Type;

use Titon\Type\Contract\Arrayable;
use Titon\Type\Contract\Jsonable;
use Titon\Type\Contract\Xmlable;
use Titon\Type\Exception\MissingMethodException;
use Titon\Utility\Converter;
use Titon\Utility\Traverse;
use \ArrayAccess;
use \Countable;
use \IteratorAggregate;
use \JsonSerializable;
use \Serializable;

/**
 * @todo
 *
 * @package Titon\Type
 * @method $this add(Tv $value)
 * @method $this addAll(Traversable<Tv> $values)
 * @method Tv at(int $index)
 * @method $this clear()
 * @method ArrayList filter((function(Tv): bool) $callback)
 * @method ArrayList filterWithKey((function(int, Tv): bool) $callback)
 * @method ArrayList fromItems(?Traversable<Tv> $items)
 * @method ?Tv get(int $index)
 * @method bool isEmpty()
 * @method Iterable<Tv> items()
 * @method Vector<int> keys()
 * @method ArrayList map((function(Tv): bool) $callback)
 * @method ArrayList mapWithKey((function(int, Tv): bool) $callback)
 * @method Tv pop()
 * @method $this reserve(int $size)
 * @method $this resize(int $size, Tv $value)
 * @method ArrayList reverse()
 * @method $this set(int $index, Tv $value)
 * @method $this setAll(KeyedTraversable<int, Tv> $values)
 * @method ArrayList shuffle()
 * @method ArrayList splice(int $offset, int $length)
 * @method array toKeysArray()
 * @method Map<int, Tv> toMap()
 * @method Set<Tv> toSet()
 * @method array toValuesArray()
 * @method Vector<Tv> toVector()
 * @method Vector<Tv> values()
 */
class ArrayList<Tv> implements ArrayAccess<int, Tv>, IteratorAggregate<Tv>, Countable, Serializable, JsonSerializable, Arrayable, Jsonable, Xmlable {

    /**
     * Methods on the Vector collection that should be chainable through ArrayList.
     *
     * @type Vector<string>
     */
    protected Vector<string> $_chainable = Vector {
        'add', 'addAll', 'clear', 'pop', 'removeKey',
        'reserve', 'resize', 'set', 'setAll'
    };

    /**
     * Methods on the Vector collection that should return a new ArrayList instance.
     *
     * @type Vector<string>
     */
    protected Vector<string> $_immutable = Vector {
        'filter', 'filterWithKey', 'fromItems', 'map', 'mapWithKey',
        'reverse', 'shuffle', 'splice'
    };

    /**
     * Raw internal Vector used for list management.
     *
     * @type Vector<Tv>
     */
    protected Vector<Tv> $_value = Vector {};

    /**
     * Set the value.
     *
     * @param Traversable<Tv> $value
     */
    public function __construct(Traversable<Tv> $value = Vector {}) {
        $this->write($value);
    }

    /**
     * Allow methods on the base Vector class to be called programmatically.
     *
     * @param string $method
     * @param array $args
     * @return mixed
     * @throws \Titon\Type\Exception\MissingMethodException
     */
    public function __call(string $method, array $args): mixed {
        $list = $this->value();

        if (method_exists($list, $method)) {
            $result = call_user_func_array(class_meth($list, $method), $args);

            // Chain the method call
            if (in_array($method, $this->_chainable)) {
                return $this;

            // Return a new instance for immutability
            } else if (in_array($method, $this->_immutable)) {
                return new static($result->toVector());
            }

            // Return the result
            return $result;
        }

        throw new MissingMethodException(sprintf('Method "%s" does not exist for %s', $method, static::class));
    }

    /**
     * Clone the internal vector so references aren't shared.
     */
    public function __clone(): void {
        $this->_value = $this->_value->toVector();
    }

    /**
     * Split a list into multiple chunked lists.
     *
     * @param int $size
     * @param bool $preserve
     * @return ArrayList<ArrayList<Tv>>
     */
    public function chunk(int $size, bool $preserve = true): ArrayList<ArrayList<Tv>> {
        $chunks = array_chunk($this->toArray(), $size, $preserve);
        $list = Vector {};

        foreach ($chunks as $chunk) {
            $list[] = new static($chunk);
        }

        return new static($list);
    }

    /**
     * Removes all empty, null, and false values.
     *
     * @return ArrayList<Tv>
     */
    public function clean(): ArrayList<Tv> {
        return $this->filter(function(Tv $value) {
            return ($value || $value === 0 || $value === '0');
        });
    }

    /**
     * Merges the current list with the defined list and returns a new ArrayList.
     *
     * @uses Titon\Utility\Traverse
     *
     * @param Traversable<Tv> $value
     * @return ArrayList<Tv>
     */
    public function concat(Traversable<Tv> $value): ArrayList<Tv> {
        return new static(Traverse::merge($this->value(), $value));
    }

    /**
     * Checks if a value exists in the list.
     *
     * @param Tv $value
     * @return bool
     */
    public function contains(Tv $value): bool {
        return in_array($value, $this->value(), true);
    }

    /**
     * Return the size of the list.
     *
     * @return int
     */
    public function count(): int {
        return $this->value()->count();
    }

    /**
     * Calculates the nested depth of the list.
     *
     * @uses Titon\Utility\Traverse
     *
     * @return int
     */
    public function depth(): int {
        return Traverse::depth($this->value());
    }

    /**
     * Apply a user function to every member of the list.
     *
     * @uses Titon\Utility\Traverse
     *
     * @param callable $callback
     * @param bool $recursive
     * @return ArrayList<Tv>
     */
    public function each(callable $callback, bool $recursive = true): ArrayList<Tv> {
        return new static(Traverse::each($this->value(), $callback, $recursive));
    }

    /**
     * Checks to see if the passed argument is an explicit exact match.
     *
     * @param Traversable<Tv> $value
     * @return bool
     */
    public function equals(Traversable<Tv> $value): bool {
        return ($this->value() === $value);
    }

    /**
     * Removes all occurrences of an item from the list.
     *
     * @param Tv $erase
     * @return $this
     */
    public function erase(Tv $erase): ArrayList<Tv> {
        $list = Vector {};

        foreach ($this->value() as $key => $value) {
            if ($value !== $erase) {
                $list[$key] = $value;
            }
        }

        return new static($list);
    }

    /**
     * Returns true if every item in the list satisfies the provided callback function.
     *
     * @uses Titon\Utility\Traverse
     *
     * @param callable $callback
     * @return bool
     */
    public function every(callable $callback): bool {
        return Traverse::every($this->value(), $callback);
    }

    /**
     * Filters items in the list recursively using a callback function.
     *
     * @uses Titon\Utility\Traverse
     *
     * @param callable $callback
     * @return ArrayList<Tv>
     */
    public function filterRecursive(callable $callback): ArrayList<Tv> {
        return new static(Traverse::filter($this->value(), true, $callback));
    }

    /**
     * Return the first item in the list.
     *
     * @return ?Tv
     */
    public function first(): ?Tv {
        return $this->isNotEmpty() ? reset($this->_value) : null;
    }

    /**
     * Empty the list.
     *
     * @return $this
     */
    public function flush(): this {
        $this->clear();

        return $this;
    }

    /**
     * Return the iterator from the Vector directly to use for looping.
     *
     * @return KeyedIterator<int, Tv>
     */
    public function getIterator(): KeyedIterator<int, Tv> {
        return $this->value()->getIterator();
    }

    /**
     * Return true if the index exists.
     *
     * @param int $index
     * @return bool
     */
    public function has(int $index): bool {
        return $this->value()->containsKey($index);
    }

    /**
     * Checks to see if the list is not empty.
     *
     * @return bool
     */
    public function isNotEmpty(): bool {
        return !$this->isEmpty();
    }

    /**
     * Return an array for JSON encoding.
     *
     * @return array
     */
    public function jsonSerialize(): array {
        return $this->toArray();
    }

    /**
     * Return the index for the first item that matches the defined value.
     * If no items are found, then -1 is returned.
     *
     * @param Tv $value
     * @return int
     */
    public function keyOf(Tv $value): int {
        return $this->value()->linearSearch($value);
    }

    /**
     * Return the last item in the list.
     *
     * @return ?Tv
     */
    public function last(): ?Tv {
        return $this->isNotEmpty() ? end($this->_value) : null;
    }

    /**
     * Alias for count().
     *
     * @return int
     */
    public function length(): int {
        return $this->count();
    }

    /**
     * Alias for concat().
     *
     * @param Traversable<Tv> $value
     * @return ArrayList<Tv>
     */
    public function merge(Traversable<Tv> $value): ArrayList<Tv> {
        return $this->concat($value);
    }

    /**
     * Return the value at the defined index.
     *
     * @param int $index
     * @param Tv $value
     * @return $this
     */
    public function offsetSet($index, $value): this {
        return $this->set($index, $value);
    }

    /**
     * Return true if a value exists at the defined index.
     *
     * @param int $index
     * @return bool
     */
    public function offsetExists($index): bool {
        return $this->has($index);
    }

    /**
     * Remove a value at the defined index.
     *
     * @param int $index
     * @return $this
     */
    public function offsetUnset($index): this {
        return $this->remove($index);
    }

    /**
     * Return a value at the defined index. If no index exists, return null.
     *
     * @param int $index
     * @return ?Tv
     */
    public function offsetGet($index): ?Tv {
        return $this->get($index);
    }

    /**
     * Remove an item at the specific index. This will reorder indices.
     *
     * @param int $index
     * @return $this
     */
    public function remove(int $index): this {
        unset($this->_value[$index]);

        return $this;
    }

    /**
     * Return the value serialized.
     *
     * @return string
     */
    public function serialize(): string {
        return serialize($this->value());
    }

    /**
     * Returns true if at least one item in the list satisfies the provided testing function.
     *
     * @uses Titon\Utility\Hash
     *
     * @param \Closure $callback
     * @return bool
     */
    public function some(Closure $callback) {
        return Hash::some($this->value(), $callback);
    }

    /**
     * Return the list as an array.
     *
     * @return array
     */
    public function toArray(): array {
        return $this->value()->toArray();
    }

    /**
     * Return the list as a JSON string.
     *
     * @param int $options
     * @return string
     */
    public function toJson(int $options = 0): string {
        return json_encode($this, $options);
    }

    /**
     * Return the list as an XML string.
     *
     * @param string $root
     * @return string
     */
    public function toXml(string $root = 'root'): string {
        return Converter::arrayToXml($this->toArray(), $root);
    }

    /**
     * Set the value after unserialization.
     *
     * @param string $value
     */
    public function unserialize($value): void {
        $this->__construct(unserialize($value));
    }

    /**
     * Return the current value.
     *
     * @return Vector<Tv>
     */
    public function value(): Vector<Tv> {
        return $this->_value;
    }

    /**
     * Removes duplicate values from the list.
     *
     * @param int $flags
     * @return ArrayList<Tv>
     */
    public function unique(int $flags = SORT_REGULAR): ArrayList<Tv> {
        return new static(new Vector(array_unique($this->toArray(), $flags)));
    }

    /**
     * Set and overwrite with a new Vector.
     *
     * @param Traversable<Tv> $value
     * @return $this
     */
    public function write(Traversable<Tv> $value): this {
        $this->_value = new Vector($value);

        return $this;
    }

}