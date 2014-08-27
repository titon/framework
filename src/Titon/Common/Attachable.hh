<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Common;

use Titon\Common\Exception\InvalidObjectException;
use Titon\Common\Exception\MissingObjectException;
use Titon\Common\Exception\UnsupportedInterfaceException;
use Titon\Utility\Inflector;
use Titon\Utility\Registry;
use Titon\Utility\Traverse;
use \Closure;

/**
 * Attachable is an inheritable trait for all classes that need dependency or functionality from other classes.
 * It allows you to attach classes to the parent class, while encapsulating the attaching class in a Closure,
 * enabling the objects to only be instantiated when triggered; also known as lazy loading.
 *
 * @package Titon\Common
 */
trait Attachable {

    /**
     * Classes and their options / namespaces to load for dependencies.
     *
     * @type Map<string, Map<string, mixed>>
     */
    protected Map<string, Map<string, mixed>> $_classes = Map {};

    /**
     * Classes that have been instantiated when called with getObject().
     *
     * @type object[]
     */
    protected Map<string, mixed> $_attached = Map {};

    /**
     * Classes that have been loaded, but are unable to be used within the current scope.
     *
     * @type Map<string, string>
     */
    protected Map<string, string> $_restricted = Map {};

    /**
     * Object map that relates a Closure object to a defined class, to allow for easy lazy-loading.
     *
     * @type Map<string, Closure>
     */
    private Map<string, Closure> $__objectMap = Map {};

    /**
     * Magic method for getObject().
     *
     * @param string $class
     * @return object|mixed
     */
    public function __get(string $class): mixed {
        return $this->getObject($class);
    }

    /**
     * Magic method for getObject().
     *
     * @param mixed $options
     * @param mixed $object
     */
    public function __set(mixed $options, ?mixed $object): void {
        $this->attachObject($options, $object);
    }

    /**
     * Magic method for hasObject().
     *
     * @param string $class
     * @return bool
     */
    public function __isset(string $class): bool {
        return $this->hasObject($class);
    }

    /**
     * Magic method for detachObject().
     *
     * @param string $class
     */
    public function __unset(string $class): void {
        $this->detachObject($class);
    }

    /**
     * Allow an object to be usable if it has been restricted. Must supply the class name.
     *
     * @param Vector<string> $classes
     * @return $this
     */
    public function allowObjects(Vector<string> $classes): this {
        foreach ($classes as $class) {
            unset($this->_restricted[$class]);
        }

        return $this;
    }

    /**
     * Attaches the defined closure object to the $__objectMap, as well as saving its options to $_classes.
     *
     * @param Map<string, mixed> $options {
     *      @type string $alias     The alias name to use for object linking
     *      @type string $class     The fully qualified class name to use for instantiation
     *      @type bool $register    Should the instance be stored in Registry
     *      @type bool $callback    Should event callbacks be allowed
     *      @type string $interface The fully qualified interface name that all all attachments must inherit
     * }
     * @param object|callable $object
     * @return $this
     * @throws \Titon\Common\Exception\InvalidObjectException
     */
    public function attachObject(mixed $options, ?mixed $object = null): this {
        if (is_string($options)) {
            $options = Map {'alias' => $options};
        }

        $options = Traverse::merge(Map {
            'alias' => null,
            'class' => null,
            'register' => true,
            'callback' => true,
            'interface' => null
        }, $options);

        if (!$options['alias']) {
            throw new InvalidObjectException('You must define an alias to reference the attached object');

        } else if ($object === null && !$options['class']) {
            throw new InvalidObjectException(sprintf('You must supply an object, Closure or a class name for %s', $options['alias']));

        } else {
            $options['alias'] = Inflector::variable($options['alias']);
        }

        // If closure
        if ($object instanceof Closure) {
            $this->__objectMap[$options['alias']] = $object;

        // If object
        } else if (is_object($object)) {
            $options['class'] = get_class($object);

            $this->_attached[$options['alias']] = $object;
        }

        $this->_classes[$options['alias']] = $options;

        return $this;
    }

    /**
     * Remove an object permanently from the $_attached, $_classes and $__objectMap properties.
     *
     * @param string $class
     * @return $this
     */
    public function detachObject(string $class): this {
        if (isset($this->_classes[$class])) {
            unset($this->_classes[$class], $this->_attached[$class], $this->__objectMap[$class]);
        }

        return $this;
    }

    /**
     * Primary method to detect if the object being called can be returned; based on restrictions and instantiation.
     * If an object is not instantiated, it will create it based off the Closure (if applicable) or the options namespace.
     *
     * @uses Titon\Utility\Registry
     *
     * @param string $class
     * @return object|mixed
     * @throws \Titon\Common\Exception\MissingObjectException
     * @throws \Titon\Common\Exception\UnsupportedInterfaceException
     */
    public function getObject(string $class): mixed {
        if (isset($this->_attached[$class])) {
            return $this->_attached[$class];

        } else if (!isset($this->_classes[$class])) {
            throw new MissingObjectException(sprintf('No object attachment could be found for %s', $class));
        }

        // Gather options
        $options = $this->_classes[$class];

        // Lazy-load the object
        if (isset($this->__objectMap[$class])) {
            $object = $this->__objectMap[$class]();

            $this->_classes[$class]['class'] = get_class($object);

        // Create manually
        } else {
            if ($options['register']) {
                $object = Registry::factory($options['class']);
            } else {
                $object = new $options['class']();
            }
        }

        if ($options['interface'] && !($object instanceof $options['interface'])) {
            throw new UnsupportedInterfaceException(sprintf('%s does not implement the %s interface', get_class($object), $options['interface']));
        }

        return $this->_attached[$class] = $object;
    }

    /**
     * Checks to see if a class has been loaded, or is present in the object map.
     *
     * @param string $class
     * @return bool
     */
    public function hasObject(string $class): bool {
        return (isset($this->_attached[$class]) || isset($this->_classes[$class]));
    }

    /**
     * Cycle through all loaded objects and trigger the defined hook method.
     *
     * @param string $method
     * @param array $args
     * @return $this
     */
    public function notifyObjects(string $method, array $args = []): this {
        if ($this->_classes) {
            foreach ($this->_classes as $options) {
                if (!$options['callback'] || in_array($options['alias'], $this->_restricted)) {
                    continue;
                }

                $object = $this->getObject($options['alias']);

                if ($object && method_exists($object, $method)) {
                    call_user_func_array([$object, $method], $args);
                }
            }
        }

        return $this;
    }

    /**
     * Restrict a class from being used within the current scope, or until the class is allowed again.
     *
     * @param Vector<string> $classes
     * @return $this
     */
    public function restrictObjects(Vector<string> $classes): this {
        foreach ($classes as $class){
            $this->_restricted[$class] = $class;
        }

        return $this;
    }

}