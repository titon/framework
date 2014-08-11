<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Type;

use Titon\Type\Exception\MissingMethodException;
use \Countable;
use \Serializable;

class ArrayList<Tv> implements ArrayAccess<int, Tv>, IteratorAggregate<Tv>, Countable, Serializable {

    /**
     * Raw vector.
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
     * @return $this
     * @throws \Titon\Type\Exception\MissingMethodException
     */
    public function __call(string $method, array $args): this {
        $list = $this->value();

        if (method_exists($list, $method)) {
            call_user_func_array(class_meth($list, $method), $args);

            return $this;
        }

        throw new MissingMethodException(sprintf('Method "%s" does not exist for %s', $method, static::class));
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
     * Alias for count().
     *
     * @return int
     */
    public function length(): int {
        return $this->count();
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
     * Return the value at the defined index.
     *
     * @param int $index
     * @param Tv $value
     * @return $this
     */
    public function offsetSet(int $index, Tv $value): this {
        $this->_value[$index] = $value;

        return $this;
    }

    /**
     * Return true if a value exists at the defined index.
     *
     * @param int $index
     * @return bool
     */
    public function offsetExists(int $index): bool {
        return isset($this->_value[$index]);
    }

    /**
     * Remove a value at the defined index.
     *
     * @param int $index
     * @return $this
     */
    public function offsetUnset(int $index): this {
        unset($this->_value[$index]);

        return $this;
    }

    /**
     * Return a value at the defined index. If no index exists, return null.
     *
     * @param int $index
     * @return Tv
     */
    public function offsetGet(int $index): ?Tv {
        return $this->value()->get($index);
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

}