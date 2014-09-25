<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Type;

use Titon\Type\Contract\Arrayable;
use Titon\Type\Contract\Jsonable;
use Titon\Type\Contract\Mapable;
use Titon\Type\Contract\Vectorable;
use Titon\Type\Contract\Xmlable;
use Titon\Type\Exception\MissingMethodException;
use Titon\Utility\Converter;
use Titon\Utility\Col;
use \ArrayAccess;
use \Countable;
use \IteratorAggregate;
use \JsonSerializable;
use \Serializable;
use \Indexish;

/**
 * The ArrayList is an immutable wrapper for integer based lists. It provides new functionality for lists through advanced methods,
 * integration with built-in Vector methods through PHP magic methods, and a easily fluent chainable API.
 *
 * @package Titon\Type
 * @method $this add(Tv $value)
 * @method $this addAll(Traversable<Tv> $values)
 * @method $this clear()
 * @method ArrayList filter((function(Tv): bool) $callback)
 * @method ArrayList filterWithKey((function(int, Tv): bool) $callback)
 * @method ArrayList map((function(Tv): bool) $callback)
 * @method ArrayList mapWithKey((function(int, Tv): bool) $callback)
 * @method $this reserve(int $size)
 * @method $this resize(int $size, Tv $value)
 * @method ArrayList reverse()
 * @method $this set(int $index, Tv $value)
 * @method $this setAll(KeyedTraversable<int, Tv> $values)
 * @method ArrayList shuffle()
 * @method ArrayList splice(int $offset, int $length)
 */
class ArrayList<Tv> implements
    IteratorAggregate<Tv>,
    Countable,
    Serializable,
    JsonSerializable,
    Arrayable<int, Tv>,
    Jsonable,
    Xmlable,
    Mapable<int, Tv>,
    Vectorable<Tv> {

    /**
     * Methods on the Vector collection that should be chainable through ArrayList.
     *
     * @type Vector<string>
     */
    protected Vector<string> $_chainable = Vector {
        'add', 'addAll', 'clear', 'removeKey',
        'reserve', 'resize', 'set', 'setAll'
    };

    /**
     * Methods on the Vector collection that should return a new ArrayList instance.
     *
     * @type Vector<string>
     */
    protected Vector<string> $_immutable = Vector {
        'filter', 'filterWithKey', 'map', 'mapWithKey',
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
     * @param Indexish<int, Tv> $value
     */
    final public function __construct(Indexish<int, Tv> $value = Vector {}) {
        $this->write($value);
    }

    /**
     * Allow methods on the base Vector class to be called programmatically.
     *
     * @param string $method
     * @param array<mixed> $args
     * @return ArrayList<Tv>
     * @throws \Titon\Type\Exception\MissingMethodException
     */
    public function __call(string $method, array<mixed> $args): ArrayList<Tv> {
        $vector = $this->value();

        if (method_exists($vector, $method)) {

            // Chain the method call
            if (in_array($method, $this->_chainable)) {

                // UNSAFE
                call_user_func_array(inst_meth($vector, $method), $args);

                return $this;

            // Return a new instance for immutability
            } else if (in_array($method, $this->_immutable)) {

                // Clone the vector so we don't interfere with references
                $clonedList = $vector->toVector();

                // UNSAFE
                $mutatedList = call_user_func_array(inst_meth($clonedList, $method), $args);

                // Some methods return void/null (reverse, etc) so use the cloned list
                if ($mutatedList === null) {
                    $mutatedList = $clonedList;
                }

                return new static($mutatedList);
            }
        }

        throw new MissingMethodException(sprintf('Method "%s" does not exist or is not callable for %s', $method, static::class));
    }

    /**
     * Clone the internal vector so references aren't shared.
     */
    public function __clone(): void {
        $this->_value = $this->_value->toVector();
    }

    /**
     * Alias for Vector::at(). Will return the value at the specified index or throw an exception.
     *
     * @param int $index
     * @return Tv
     */
    public function at(int $index): Tv {
        return $this->value()->at($index);
    }

    /**
     * Split a list into multiple chunked lists.
     *
     * @param int $size
     * @return ArrayList<ArrayList<Tv>>
     */
    public function chunk(int $size): ArrayList<ArrayList<Tv>> {
        $chunks = array_chunk($this->toArray(), $size);
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
     * Merges the current ArrayList with the another ArrayList and returns a new ArrayList.
     * Can either prepend or append the defined list.
     *
     * @param ArrayList<Tv> $value
     * @param bool $append
     * @return ArrayList<Tv>
     */
    public function concat(ArrayList<Tv> $value, bool $append = true): ArrayList<Tv> {
        $oldList = $this->toVector();
        $newList = $value->toVector();

        if ($append) {
            $list = $oldList->addAll($newList);
        } else {
            $list = $newList->addAll($oldList);
        }

        return new static($list);
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
     * @uses Titon\Utility\Col
     *
     * @return int
     */
    public function depth(): int {
        return Col::depth($this->value());
    }

    /**
     * Apply a user function to every member of the list.
     *
     * @uses Titon\Utility\Col
     *
     * @param (function(int, Tv): mixed) $callback
     * @param bool $recursive
     * @return ArrayList<Tv>
     */
    public function each((function(int, Tv): mixed) $callback, bool $recursive = true): ArrayList<Tv> {
        return new static(Col::each($this->value(), $callback, $recursive));
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
                $list[] = $value;
            }
        }

        return new static($list);
    }

    /**
     * Returns true if every item in the list satisfies the provided callback function.
     *
     * @uses Titon\Utility\Col
     *
     * @param (function(int, Tv): bool) $callback
     * @return bool
     */
    public function every((function(int, Tv): bool) $callback): bool {
        return Col::every($this->value(), $callback);
    }

    /**
     * Return the first item in the list.
     *
     * @return ?Tv
     */
    public function first(): ?Tv {
        return $this->get(0);
    }

    /**
     * Alias for Vector::clear(). Empty the list.
     *
     * @return $this
     */
    public function flush(): this {
        $this->clear();

        return $this;
    }

    /**
     * Alias for Vector::get().  Will return the value at the specified index or return null.
     *
     * @param int $index
     * @return ?Tv
     */
    public function get(int $index): ?Tv {
        return $this->value()->get($index);
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
     * Alias for Vector::isEmpty(). Will return true if the list is empty.
     *
     * @return bool
     */
    public function isEmpty(): bool {
        return $this->value()->isEmpty();
    }

    /**
     * Return an array for JSON encoding.
     *
     * @return array<int, Tv>
     */
    public function jsonSerialize(): array<int, Tv> {
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
     * Alias for Vector::keys(). Return a vector containing the list of keys.
     *
     * @return Vector<int>
     */
    public function keys(): Vector<int> {
        return $this->value()->keys();
    }

    /**
     * Return the last item in the list.
     *
     * @return ?Tv
     */
    public function last(): ?Tv {
        return $this->get($this->count() - 1);
    }

    /**
     * Alias for Vector::count(). Return the length of the list.
     *
     * @return int
     */
    public function length(): int {
        return $this->count();
    }

    /**
     * Merge two ArrayLists together with values from the second list overwriting the first list.
     *
     * @param ArrayList<Tv> $value
     * @return ArrayList<Tv>
     */
    public function merge(ArrayList<Tv> $value): ArrayList<Tv> {
        return new static($this->toVector()->setAll($value->toVector()));
    }

    /**
     * Remove an item at the specific index. This will reorder indices.
     *
     * @param int $index
     * @return $this
     */
    public function remove(int $index): this {
        $this->_value->removeKey($index);

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
     * @uses Titon\Utility\Col
     *
     * @param (function(int, Tv): bool) $callback
     * @return bool
     */
    public function some((function(int, Tv): bool) $callback): bool {
        return Col::some($this->value(), $callback);
    }

    /**
     * Sort the items in the list using a custom callback or the default sorting mechanism.
     *
     * @param (function(Tv, Tv): int) $callback
     * @param int $flags
     * @return ArrayList<Tv>
     */
    public function sort(?(function(Tv, Tv): int) $callback = null, int $flags = SORT_REGULAR): ArrayList<Tv> {
        $list = $this->toVector();

        if ($callback) {
            usort($list, $callback);
        } else {
            sort($list, $flags);
        }

        return new static($list);
    }

    /**
     * Sort a multi-dimensional list by comparing a field within each item.
     *
     * @param string $key
     * @param int $flags
     * @return ArrayList<Tv>
     */
    public function sortBy(string $key, int $flags = SORT_REGULAR): ArrayList<Tv> {
        return $this->sort(($a, $b) ==> {
            if ($a instanceof Indexish && $b instanceof Indexish) {
                return strcmp($a[$key], $b[$key]);
            }

            return strcmp((string) $a, (string) $b);
        }, $flags);
    }

    /**
     * Sort the array using a natural algorithm. This function implements a sort algorithm that orders
     * alphanumeric strings in the way a human being would.
     *
     * @param bool $sensitive
     * @return ArrayList<Tv>
     */
    public function sortNatural(bool $sensitive = false): ArrayList<Tv> {
        $list = $this->toArray();

        if ($sensitive) {
            natsort($list);
        } else {
            natcasesort($list);
        }

        return new static(array_values($list));
    }

    /**
     * Return the list as an array.
     *
     * @return array<int, Tv>
     */
    public function toArray(): array<int, Tv> {
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
     * Return the list as a map.
     *
     * @return Map<int, Tv>
     */
    public function toMap(): Map<int, Tv> {
        return $this->value()->toMap();
    }

    /**
     * Return the list as a vector.
     *
     * @return Vector<Tv>
     */
    public function toVector(): Vector<Tv> {
        return $this->value()->toVector();
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
        $this->write(unserialize($value));
    }

    /**
     * Removes duplicate values from the list.
     *
     * @param int $flags
     * @return ArrayList<Tv>
     */
    public function unique(int $flags = SORT_REGULAR): ArrayList<Tv> {
        return new static(array_unique($this->toArray(), $flags));
    }

    /**
     * Return the raw value.
     *
     * @return Vector<Tv>
     */
    public function value(): Vector<Tv> {
        return $this->_value;
    }

    /**
     * Alias for Vector::values(). Return a vector containing the list of values.
     *
     * @return Vector<Tv>
     */
    public function values(): Vector<Tv> {
        return $this->value()->values();
    }

    /**
     * Set and overwrite with a new Vector.
     *
     * @param Indexish<int, Tv> $value
     * @return $this
     */
    public function write(Indexish<int, Tv> $value): this {
        $this->_value = new Vector($value);

        return $this;
    }

}