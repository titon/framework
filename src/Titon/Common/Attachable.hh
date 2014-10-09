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

type ClassMap = Map<string, Map<string, mixed>>;
type AttachedMap = Map<string, mixed>;
type ObjectCallback = (function(): mixed);
type ObjectMap = Map<string, ObjectCallback>;
type RestrictedMap = Map<string, string>;

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
     * @type \Titon\Common\ClassMap
     */
    protected ClassMap $_classes = Map {};

    /**
     * Classes that have been instantiated when called with getObject().
     *
     * @type \Titon\Common\AttachedMap
     */
    protected AttachedMap $_attached = Map {};

    /**
     * Classes that have been loaded, but are unable to be used within the current scope.
     *
     * @type \Titon\Common\RestrictedMap
     */
    protected RestrictedMap $_restricted = Map {};

    /**
     * Object map that relates a Closure object to a defined class, to allow for easy lazy-loading.
     *
     * @type \Titon\Common\ObjectMap
     */
    private ObjectMap $__objectMap = Map {};

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
    public function __set(mixed $options, mixed $object): void {
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
            $this->_restricted->remove($class);
        }

        return $this;
    }

    /**
     * Attaches the defined closure object to the $__objectMap, as well as saving its options to $_classes.
     *
     * @param \Titon\Common\OptionMap $options {
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
    public function attachObject(mixed $options, mixed $object = null): this {
        if (!$options instanceof Map) {
            $options = Map {'alias' => $options};
        }

        invariant($options instanceof Map, 'Options need to be a map');

        $options = (Map {
            'alias' => '',
            'class' => '',
            'register' => true,
            'callback' => true,
            'interface' => ''
        })->setAll($options);

        if (!$options['alias']) {
            throw new InvalidObjectException('You must define an alias to reference the attached object');

        } else if ($object === null && !$options['class']) {
            throw new InvalidObjectException(sprintf('You must supply an object, callable or a class name for %s', $options['alias']));

        } else {
            $options['alias'] = Inflector::variable($options['alias']);
        }

        // If closure
        if (is_callable($object)) {
            invariant($object instanceof Closure, 'Object must be a Closure');

            $this->__objectMap[(string) $options['alias']] = $object;

        // If object
        } else if (is_object($object)) {
            $options['class'] = get_class($object);

            $this->_attached[(string) $options['alias']] = $object;
        }

        $this->_classes[(string) $options['alias']] = $options;

        return $this;
    }

    /**
     * Remove an object permanently from the $_attached, $_classes and $__objectMap properties.
     *
     * @param string $class
     * @return $this
     */
    public function detachObject(string $class): this {
        if ($this->_classes->contains($class)) {
            $this->_classes->remove($class);
            $this->_attached->remove($class);
            $this->__objectMap->remove($class);
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
        if ($this->_attached->contains($class)) {
            return $this->_attached[$class];

        } else if (!$this->_classes->contains($class)) {
            throw new MissingObjectException(sprintf('No object attachment could be found for %s', $class));
        }

        // Gather options
        $options = $this->_classes[$class];

        // Lazy-load the object
        if ($this->__objectMap->contains($class)) {
            $object = call_user_func($this->__objectMap[$class]);

            $this->_classes[$class]['class'] = get_class($object);

        // Create manually
        } else {
            $object = Registry::factory($options['class'], Vector {}, (bool) $options['register']);
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
        return ($this->_attached->contains($class) || $this->_classes->contains($class));
    }

    /**
     * Cycle through all loaded objects and trigger the defined hook method.
     *
     * @param string $method
     * @param array<mixed> $args
     * @return $this
     */
    public function notifyObjects(string $method, array<mixed> $args = []): this {
        if ($this->_classes) {
            foreach ($this->_classes as $options) {
                if (!$options['callback'] || in_array($options['alias'], $this->_restricted)) {
                    continue;
                }

                $object = $this->getObject((string) $options['alias']);

                if (is_object($object) && method_exists($object, $method)) {
                    // UNSAFE
                    call_user_func_array(inst_meth($object, $method), $args);
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