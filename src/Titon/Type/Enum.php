<?hh
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Type;

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

    /**
     * Constant caching.
     *
     * @type array
     */
    protected static $_cache = [];

    /**
     * Mapping of enum constructor arguments.
     *
     * @type array
     */
    protected $_enums = [];

    /**
     * The current enum type.
     *
     * @type int
     */
    protected $_type;

    /**
     * Construct the enum based on the class constants.
     * Initialize the arguments if any exist.
     *
     * @param int $type
     * @throws \Titon\Type\Exception\InvalidEnumerableException
     */
    final public function __construct($type) {
        $keys = static::keys();

        if (!is_int($type) || !isset($keys[$type])) {
            throw new InvalidEnumerableException(sprintf('Invalid enum type detected for %s', get_class($this)));
        }

        $this->_type = (int) $type;

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
    final public static function __callStatic($method, $args) {
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
    final public function __toString() {
        return $this->name();
    }

    /**
     * Validate the current type matches one of the enum constants.
     *
     * @param int $type
     * @return bool
     */
    final public function is($type) {
        return ($this->_type === $type);
    }

    /**
     * Returns the names (or keys) of all of constants in the enum.
     *
     * @return array
     */
    final public static function keys() {
        return array_flip(static::values());
    }

    /**
     * Return type constant name.
     *
     * @return string
     */
    final public function name() {
        $keys = static::keys();
        return $keys[$this->_type];
    }

    /**
     * Return the selected types value.
     *
     * @return int
     */
    final public function value() {
        return $this->_type;
    }

    /**
     * Return the names and values of all the constants in the enum.
     *
     * @return array
     */
    final public static function values() {
        $class = get_called_class();

        if (isset(static::$_cache[$class])) {
            return static::$_cache[$class];
        }

        $reflected = new ReflectionClass($class);

        return static::$_cache[$class] = $reflected->getConstants();
    }

}
