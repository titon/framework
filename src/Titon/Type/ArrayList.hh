<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Type;

class ArrayList<T> implements Collection<T>, KeyedIterable<int, T> {

    /**
     * Raw vector.
     *
     * @type Vector<T>
     */
    protected Vector<T> $_value = Vector {};

    /**
     * Set the value.
     *
     * @param Traversable<T> $value
     */
    public function __construct(Traversable<T> $value = Vector {}) {
        $this->write($value);
    }

    /**
     * Add an item to the list.
     *
     * @param T $item
     * @return $this
     */
    public function add(T $item): this {
        $this->value()->add($item);

        return $this;
    }

    /**
     * Add multiple items to the list.
     *
     * @param Traversable<T> $iterable
     * @return $this
     */
    public function addAll(?Traversable<T> $items): this {
        $this->value->addAll($items);

        return $this;
    }

    public function at(int $k): Tv {

    }

    /**
     * Empty the list.
     *
     * @return $this
     */
    public function clear(): this {
        $this->value()->clear();

        return $this;
    }

    public function contains(mixed $value) {

    }

    public function containsKey(int $index): bool {

    }

    /**
     * Return a count for the number of items in the list.
     *
     * @return int
     */
    public function count(): int {
        return $this->value()->count();
    }

    public function filter((function(T): bool) $fn): KeyedIterable<int, T> {
        return $this->value()->filter($fn);
    }

    public function filterWithKey((function(int, T): bool) $fn):KeyedIterable<int, T> {
        return $this->value()->filterWithKey($fn);
    }

    public function getIterator(): KeyedIterator<int, T> {
        return $this->value()->getIterator();
    }

    /**
     * Return the items for iteration.
     *
     * @return Iterable<T>
     */
    public function items(): Iterable<T> {
        return $this->value()->items();
    }

    /**
     * Return true if the list contains no items.
     *
     * @return bool
     */
    public function isEmpty(): bool {
        return $this->value()->isEmpty();
    }

    public function keys(): Iterable<T> {
        return $this->value()->keys();
    }

    public function map<Tu>((function(T): Tu) $fn): KeyedIterable<int, Tu> {
        return $this->value()->map($fn);
    }

    public function mapWithKey<Tu>((function(int, T): Tu) $fn): KeyedIterable<int, Tu> {
        return $this->value()->mapWithKey($fn);
    }

    /**
     * Return the current value.
     *
     * @return Vector<T>
     */
    public function value(): Vector<T> {
        return $this->_value;
    }

    public function values(): Iterable<T> {
        return $this->value()->values();
    }

    /**
     * Set or overwrite the value.
     *
     * @param Traversable<T> $value
     * @return $this
     */
    public function write(Traversable<T> $value): this {
        $this->_value = new Vector($value);

        return $this;
    }

}