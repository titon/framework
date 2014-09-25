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
 * The HashMap is an immutable wrapper for key-value based lists. It provides new functionality for lists through advanced methods,
 * integration with built-in Map methods through PHP magic methods, and a easily fluent chainable API.
 *
 * @package Titon\Type
 * @method $this add(Pair<Tk, Tv> $value)
 * @method $this addAll(Traversable<Pair<Tk, Tv>> $values)
 * @method $this clear()
 * @method HashMap filter((function(Tv): bool) $callback)
 * @method HashMap filterWithKey((function(Tk, Tv): bool) $callback)
 * @method HashMap map((function(Tv): Tu) $callback)
 * @method HashMap mapWithKey((function(Tk, Tv): Tu) $callback)
 * @method $this set(Tk $key, Tv $value)
 * @method $this setAll(KeyedTraversable<Tk, Tv> $values)
 */
class HashMap<Tk, Tv> implements
    IteratorAggregate<Tv>,
    Countable,
    Serializable,
    JsonSerializable,
    Arrayable<Tk, Tv>,
    Jsonable,
    Xmlable,
    Mapable<Tk, Tv>,
    Vectorable<Tv> {

    /**
     * Methods on the Map collection that should be chainable through HashMap.
     *
     * @type Vector<string>
     */
    protected Vector<string> $_chainable = Vector {
        'add', 'addAll', 'clear', 'removeKey',
        'reserve', 'resize', 'set', 'setAll'
    };

    /**
     * Methods on the Map collection that should return a new HashMap instance.
     *
     * @type Vector<string>
     */
    protected Vector<string> $_immutable = Vector {
        'filter', 'filterWithKey', 'map', 'mapWithKey',
        'reverse', 'shuffle', 'splice'
    };

    /**
     * Raw internal Map used for list management.
     *
     * @type Map<Tk, Tv>
     */
    protected Map<Tk, Tv> $_value = Map {};

    /**
     * Set the value.
     *
     * @param Indexish<Tk, Tv> $value
     */
    final public function __construct(Indexish<Tk, Tv> $value = Map {}) {
        $this->write($value);
    }

    /**
     * Allow methods on the base Map class to be called programmatically.
     *
     * @param string $method
     * @param array<mixed> $args
     * @return HashMap<Tk, Tv>
     * @throws \Titon\Type\Exception\MissingMethodException
     */
    public function __call(string $method, array<mixed> $args): HashMap<Tk, Tv> {
        $map = $this->value();

        if (method_exists($map, $method)) {

            // Chain the method call
            if (in_array($method, $this->_chainable)) {

                // UNSAFE
                call_user_func_array(inst_meth($map, $method), $args);

                return $this;

            // Return a new instance for immutability
            } else if (in_array($method, $this->_immutable)) {

                // Clone the map so we don't interfere with references
                $clonedList = $map->toMap();

                // UNSAFE
                $mutatedList = call_user_func_array(inst_meth($clonedList, $method), $args);

                // Some methods return void/null so use the cloned list
                if ($mutatedList === null) {
                    $mutatedList = $clonedList;
                }

                return new static($mutatedList);
            }
        }

        throw new MissingMethodException(sprintf('Method "%s" does not exist or is not callable for %s', $method, static::class));
    }

    /**
     * Clone the internal map so references aren't shared.
     */
    public function __clone(): void {
        $this->_value = $this->_value->toMap();
    }

    /**
     * Alias for Map::at(). Will return the value at the specified index or throw an exception.
     *
     * @param Tk $key
     * @return Tv
     */
    public function at(Tk $key): Tv {
        return $this->value()->at($key);
    }

    /**
     * Split a map into multiple chunked maps.
     *
     * @param int $size
     * @return ArrayList<HashMap<Tk, Tv>>
     */
    public function chunk(int $size): ArrayList<HashMap<Tk, Tv>> {
        $chunks = array_chunk($this->toArray(), $size, true);
        $list = Vector {};

        foreach ($chunks as $chunk) {
            $list[] = new static($chunk);
        }

        return new ArrayList($list);
    }

    /**
     * Removes all empty, null, and false values.
     *
     * @return HashMap<Tk, Tv>
     */
    public function clean(): HashMap<Tk, Tv> {
        return $this->filter(function(Tv $value) {
            return ($value || $value === 0 || $value === '0');
        });
    }

    /**
     * Merges the current HashMap with the another HashMap and returns a new HashMap.
     * Can either prepend or append the defined list.
     *
     * @param HashMap<Tk, Tv> $value
     * @param bool $append
     * @return HashMap<Tk, Tv>
     */
    public function concat(HashMap<Tk, Tv> $value, bool $append = true): HashMap<Tk, Tv> {
        $oldMap = $this->toMap();
        $newMap = $value->toMap();

        if ($append) {
            $map = $oldMap->addAll($newMap->items());
        } else {
            $map = $newMap->addAll($oldMap->items());
        }

        return new static($map);
    }

    /**
     * Checks if a value exists in the map.
     *
     * @param Tv $value
     * @return bool
     */
    public function contains(Tv $value): bool {
        return in_array($value, $this->value(), true);
    }

    /**
     * Return the size of the map.
     *
     * @return int
     */
    public function count(): int {
        return $this->value()->count();
    }

    /**
     * Calculates the nested depth of the map.
     *
     * @uses Titon\Utility\Col
     *
     * @return int
     */
    public function depth(): int {
        return Col::depth($this->value());
    }

    /**
     * Apply a user function to every member of the map.
     *
     * @uses Titon\Utility\Col
     *
     * @param (function(Tk, Tv): mixed) $callback
     * @param bool $recursive
     * @return HashMap<Tk, Tv>
     */
    public function each((function(Tk, Tv): mixed) $callback, bool $recursive = true): HashMap<Tk, Tv> {
        return new static(Col::each($this->value(), $callback, $recursive));
    }

    /**
     * Removes all occurrences of an item from the map.
     *
     * @param Tv $erase
     * @return HashMap<Tk, Tv>
     */
    public function erase(Tv $erase): HashMap<Tk, Tv> {
        $map = Map {};

        foreach ($this->value() as $key => $value) {
            if ($value !== $erase) {
                $map[$key] = $value;
            }
        }

        return new static($map);
    }

    /**
     * Returns true if every item in the map satisfies the provided callback function.
     *
     * @uses Titon\Utility\Col
     *
     * @param (function(Tk, Tv): bool) $callback
     * @return bool
     */
    public function every((function(Tk, Tv): bool) $callback): bool {
        return Col::every($this->value(), $callback);
    }

    /**
     * Return the first item in the map.
     *
     * @return ?Tv
     */
    public function first(): ?Tv {
        $keys = $this->keys();

        if ($keys->containsKey(0)) {
            return $this->get($keys[0]);
        }

        return null;
    }

    /**
     * Alias for Map::clear(). Empty the map.
     *
     * @return $this
     */
    public function flush(): this {
        $this->clear();

        return $this;
    }

    /**
     * Alias for Map::get(). Will return the value at the specified index or return null.
     *
     * @param Tk $key
     * @return ?Tv
     */
    public function get(Tk $key): ?Tv {
        return $this->value()->get($key);
    }

    /**
     * Return the iterator from the Map directly to use for looping.
     *
     * @return KeyedIterator<Tk, Tv>
     */
    public function getIterator(): KeyedIterator<Tk, Tv> {
        return $this->value()->getIterator();
    }

    /**
     * Return true if the index exists.
     *
     * @param Tk $key
     * @return bool
     */
    public function has(Tk $key): bool {
        return $this->value()->containsKey($key);
    }

    /**
     * Returns the index in which the passed key exists.
     * Returns -1 if the key does not exist.
     *
     * @param Tk $key
     * @return int
     */
    public function indexOf(Tk $key): int {
        $count = 0;

        foreach ($this->value() as $index => $value) {
            if ($index === $key) {
                return $count;
            }

            ++$count;
        }

        return -1;
    }

    /**
     * Alias for Map::isEmpty(). Will return true if the map is empty.
     *
     * @return bool
     */
    public function isEmpty(): bool {
        return $this->value()->isEmpty();
    }

    /**
     * Return an array for JSON encoding.
     *
     * @return array<Tk, Tv>
     */
    public function jsonSerialize(): array<Tk, Tv> {
        return $this->toArray();
    }

    /**
     * Return the key for the first item that matches the defined value.
     * If no items are found, then null is returned.
     *
     * @param Tv $value
     * @return Tk
     */
    public function keyOf(Tv $value): ?Tk {
        foreach ($this->getIterator() as $key => $val) {
            if ($value === $val) {
                return $key;
            }
        }

        return null;
    }

    /**
     * Alias for Map::keys(). Return a vector containing the list of keys.
     *
     * @return Vector<Tk>
     */
    public function keys(): Vector<Tk> {
        return $this->value()->keys();
    }

    /**
     * Return the last item in the map.
     *
     * @return ?Tv
     */
    public function last(): ?Tv {
        $keys = $this->keys();
        $count = $this->count();

        if ($count > 0) {
            return $this->get($keys[$count - 1]);
        }

        return null;
    }

    /**
     * Alias for Map::count(). Return the length of the map.
     *
     * @return int
     */
    public function length(): int {
        return $this->count();
    }

    /**
     * Merge two HashMaps together with values from the second map overwriting the first map.
     *
     * @param HashMap<Tk, Tv> $value
     * @return HashMap<Tk, Tv>
     */
    public function merge(HashMap<Tk, Tv> $value): HashMap<Tk, Tv> {
        return new static($this->toMap()->setAll($value->toMap()));
    }

    /**
     * Remove an item at the specific index. This will reorder indices.
     *
     * @param Tk $key
     * @return $this
     */
    public function remove(Tk $key): this {
        $this->_value->removeKey($key);

        return $this;
    }

    /**
     * Reverse the items in the map.
     *
     * @return HashMap<Tk, Tv>
     */
    public function reverse(): HashMap<Tk, Tv> {
        return new static(array_reverse($this->toArray(), true));
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
     * Shuffle the items in the map into a random order.
     *
     * @return HashMap<Tk, Tv>
     */
    public function shuffle(): HashMap<Tk, Tv> {
        $map = $this->toArray();
        shuffle($map);

        return new static($map);
    }

    /**
     * Returns true if at least one item in the map satisfies the provided testing function.
     *
     * @uses Titon\Utility\Col
     *
     * @param (function(Tk, Tv): bool) $callback
     * @return bool
     */
    public function some((function(Tk, Tv): bool) $callback): bool {
        return Col::some($this->value(), $callback);
    }

    /**
     * Sort the items in the map using a custom callback or the default sorting mechanism.
     *
     * @param (function(Tv, Tv): int) $callback
     * @param int $flags
     * @return HashMap<Tk, Tv>
     */
    public function sort(?(function(Tv, Tv): int) $callback = null, int $flags = SORT_REGULAR): HashMap<Tk, Tv> {
        $map = $this->toMap();

        if ($callback) {
            uasort($map, $callback);
        } else {
            asort($map, $flags);
        }

        return new static($map);
    }

    /**
     * Sort a multi-dimensional map by comparing a field within each item.
     *
     * @param string $key
     * @param int $flags
     * @return HashMap<Tk, Tv>
     */
    public function sortBy(string $key, int $flags = SORT_REGULAR): HashMap<Tk, Tv> {
        return $this->sort(($a, $b) ==> {
            if ($a instanceof Indexish && $b instanceof Indexish) {
                return strcmp($a[$key], $b[$key]);
            }

            return strcmp((string) $a, (string) $b);
        }, $flags);
    }

    /**
     * Return the map as an array.
     *
     * @return array<Tk, Tv>
     */
    public function toArray(): array<Tk, Tv> {
        return $this->value()->toArray();
    }

    /**
     * Return the map as a JSON string.
     *
     * @param int $options
     * @return string
     */
    public function toJson(int $options = 0): string {
        return json_encode($this, $options);
    }

    /**
     * Return the map as a Map.
     *
     * @return Map<Tk, Tv>
     */
    public function toMap(): Map<Tk, Tv> {
        return $this->value()->toMap();
    }

    /**
     * Return the map as a vector.
     *
     * @return Vector<Tv>
     */
    public function toVector(): Vector<Tv> {
        return $this->value()->toVector();
    }

    /**
     * Return the map as an XML string.
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
     * Removes duplicate values from the map.
     *
     * @param int $flags
     * @return HashMap<Tk, Tv>
     */
    public function unique(int $flags = SORT_REGULAR): HashMap<Tk, Tv> {
        return new static(array_unique($this->toArray(), $flags));
    }

    /**
     * Return the raw value.
     *
     * @return Map<Tk, Tv>
     */
    public function value(): Map<Tk, Tv> {
        return $this->_value;
    }

    /**
     * Alias for Map::values(). Return a vector containing the list of values.
     *
     * @return Vector<Tv>
     */
    public function values(): Vector<Tv> {
        return $this->value()->values();
    }

    /**
     * Set and overwrite with a new Map.
     *
     * @param Indexish<Tk, Tv> $value
     * @return $this
     */
    public function write(Indexish<Tk, Tv> $value): this {
        $this->_value = new Map($value);

        return $this;
    }

}