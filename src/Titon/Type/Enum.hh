<?hh //strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Type;

use Titon\Common\StaticCacheable;
use Titon\Type\Exception\InvalidEnumerableException;
use \ReflectionClass;

/**
 * The Enum type provides a basic interface to mimic enum based classes. Enums are first defined using class constants,
 * with their values denoted as integers, and finally added as an argument to the $_enums property. This property also
 * accepts an array of arguments that will be triggered using initialize() when the class is instantiated.
 *
 * @package Titon\Type
 */
class Enum {
    use StaticCacheable;

    /**
     * Mapping of enum constructor arguments.
     *
     * @type Vector<mixed>
     */
    protected Vector<mixed> $_enums = Vector {};

    /**
     * The current enum type.
     *
     * @type int
     */
    protected int $_type;

    /**
     * Construct the enum based on the class constants.
     * Initialize the arguments if any exist.
     *
     * @param int $type
     * @throws \Titon\Type\Exception\InvalidEnumerableException
     */
    final public function __construct(int $type) {
        $keys = static::keys();

        if (!isset($keys[$type])) {
            throw new InvalidEnumerableException(sprintf('Invalid enum type detected for %s', static::class));
        }

        $this->_type = $type;

        if (method_exists($this, 'initialize') && isset($this->_enums[$type])) {
            call_user_func_array([$this, 'initialize'], $this->_enums[$type]);
        }
    }

    /**
     * Return an instance of the enum class when the constant name is called as a method.
     * For example, Color::BLACK() is equivalent to new Color(Color::BLACK).
     *
     * @param string $method
     * @param array $args
     * @return \Titon\Type\Enum
     * @throws \Titon\Type\Exception\InvalidEnumerableException
     */
    final public static function __callStatic(string $method, array $args): Enum {
        if (defined("static::$method")) {
            return new static(constant("static::$method"));
        }

        throw new InvalidEnumerableException(sprintf('Enumerable %s does not exist', $method));
    }

    /**
     * Alias for toString().
     *
     * @return string
     */
    final public function __toString(): string {
        return $this->name();
    }

    /**
     * Validate the current type matches one of the enum constants.
     *
     * @param int $type
     * @return bool
     */
    final public function is(int $type): bool {
        return ($this->_type === $type);
    }

    /**
     * Returns the names (or keys) of all of constants in the enum.
     *
     * @return Vector<string>
     */
    final public static function keys(): Vector<string> {
        return static::values()->keys();
    }

    /**
     * Return type constant name.
     *
     * @return string
     */
    final public function name(): string {
        return static::keys()[$this->_type];
    }

    /**
     * Return the selected types value.
     *
     * @return int
     */
    final public function value(): int {
        return $this->_type;
    }

    /**
     * Return the names and values of all the constants in the enum.
     *
     * @return Map<string, int>
     */
    final public static function values(): Map<string, int> {
        $class = static::class;

        return static::cache([__METHOD__, $class], function() use ($class) {
            return new Map((new ReflectionClass($class))->getConstants());
        });
    }

}
