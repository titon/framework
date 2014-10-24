<?hh // strict
/**
 * @copyright   2010-2014, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Common\Bag;

use Titon\Common\Exception\InvalidDescriptorException;
use Titon\Utility\Path;
use \ReflectionClass;
use \ReflectionMethod;
use \ReflectionProperty;

/**
 * A bag that supplies meta data about the current class by using the reflection API.
 * This data includes path location, method and property information,
 * class name variants, so on and so forth.
 *
 * @package Titon\Common\Bag
 */
class ReflectionBag<Tk, Tv> extends AbstractBag<Tk, Tv> {

    /**
     * Class to introspect.
     *
     * @type object
     */
    protected mixed $_class;

    /**
     * Reflection object.
     *
     * @type \ReflectionClass
     */
    protected ReflectionClass $_reflection;

    /**
     * Store the class to grab information on and its reflection.
     *
     * @param object $class
     */
    public function __construct(mixed $class) {
        parent::__construct();

        $this->_class = $class;
        $this->_reflection = new ReflectionClass($class);
    }

    /**
     * Return a reflected class value. If no value exists, attempt to generate a value.
     *
     * @param string $key
     * @param mixed $default
     * @return mixed
     * @throws \Titon\Common\Exception\InvalidDescriptorException
     */
    public function get(Tk $key, ?Tv $default = null): ?Tv {
        if ($this->has($key)) {
            return parent::get($key);
        }

        if (method_exists($this, $key)) {
            // UNSAFE
            $value = call_user_func(inst_meth($this, $key));

            $this->set($key, $value);

            return $value;
        }

        throw new InvalidDescriptorException(sprintf('Reflection descriptor %s does not exist', $key));
    }

    /**
     * Return the reflection object.
     *
     * @return \ReflectionClass
     */
    public function reflection(): ReflectionClass {
        return $this->_reflection;
    }

    /**
     * Return the class name with the namespace.
     *
     * @return string
     */
    public function className(): string {
        return $this->reflection()->getName();
    }

    /**
     * Return the class name without the namespace.
     *
     * @return string
     */
    public function shortClassName(): string {
        return $this->reflection()->getShortName();
    }

    /**
     * Return the namespace without the class name.
     *
     * @return string
     */
    public function namespaceName(): string {
        return $this->reflection()->getNamespaceName();
    }

    /**
     * Return the file system path to the class.
     *
     * @return string
     */
    public function filePath(): string {
        return Path::toPath(get_class($this->_class));
    }

    /**
     * Return an array of public, protected, private and static methods.
     *
     * @return Vector<string>
     */
    public function methods(): Vector<string> {
        $methods = Vector {};
        $methods->addAll($this->publicMethods());
        $methods->addAll($this->protectedMethods());
        $methods->addAll($this->privateMethods());
        $methods->addAll($this->staticMethods());

        return new Vector(array_unique($methods->toValuesArray()));
    }

    /**
     * Return an array of public methods.
     *
     * @return Vector<string>
     */
    public function publicMethods(): Vector<string> {
        return $this->_methods(ReflectionMethod::IS_PUBLIC);
    }

    /**
     * Return an array of protected methods.
     *
     * @return Vector<string>
     */
    public function protectedMethods(): Vector<string> {
        return $this->_methods(ReflectionMethod::IS_PROTECTED);
    }

    /**
     * Return an array of private methods.
     *
     * @return Vector<string>
     */
    public function privateMethods(): Vector<string> {
        return $this->_methods(ReflectionMethod::IS_PRIVATE);
    }

    /**
     * Return an array of static methods.
     *
     * @return Vector<string>
     */
    public function staticMethods(): Vector<string> {
        return $this->_methods(ReflectionMethod::IS_STATIC);
    }

    /**
     * Return an array of public, protected, private and static properties.
     *
     * @return Vector<string>
     */
    public function properties(): Vector<string> {
        $properties = Vector {};
        $properties->addAll($this->publicProperties());
        $properties->addAll($this->protectedProperties());
        $properties->addAll($this->privateProperties());
        $properties->addAll($this->staticProperties());

        return new Vector(array_unique($properties->toValuesArray()));
    }

    /**
     * Return an array of public properties.
     *
     * @return Vector<string>
     */
    public function publicProperties(): Vector<string> {
        return $this->_properties(ReflectionProperty::IS_PUBLIC);
    }

    /**
     * Return an array of protected properties.
     *
     * @return Vector<string>
     */
    public function protectedProperties(): Vector<string> {
        return $this->_properties(ReflectionProperty::IS_PROTECTED);
    }

    /**
     * Return an array of private properties.
     *
     * @return Vector<string>
     */
    public function privateProperties(): Vector<string> {
        return $this->_properties(ReflectionProperty::IS_PRIVATE);
    }

    /**
     * Return an array of static properties.
     *
     * @return Vector<string>
     */
    public function staticProperties(): Vector<string> {
        return $this->_properties(ReflectionProperty::IS_STATIC);
    }

    /**
     * Return an array of constants defined in the class.
     *
     * @return Map<string, mixed>
     */
    public function constants(): Map<string, mixed> {
        return new Map($this->reflection()->getConstants());
    }

    /**
     * Return an array of interfaces that the class implements.
     *
     * @return Vector<string>
     */
    public function interfaces(): Vector<string> {
        return new Vector($this->reflection()->getInterfaceNames());
    }

    /**
     * Return an array of traits that the class implements.
     *
     * @return Vector<string>
     */
    public function traits(): Vector<string> {
        $traits = new Vector($this->reflection()->getTraitNames());
        $parent = get_parent_class($this->_class);

        while ($parent) {
            $traits->addAll(class_uses($parent));
            $parent = get_parent_class($parent);
        }

        return $traits;
    }

    /**
     * Return the parent class name.
     *
     * @return string
     */
    public function parent(): string {
        $parent = $this->reflection()->getParentClass();

        return ($parent instanceof ReflectionClass) ? $parent->getName() : '';
    }

    /**
     * Return an array of methods for the defined scope.
     *
     * @param int $scope
     * @return Vector<string>
     */
    protected function _methods(int $scope): Vector<string> {
        $methods = Vector {};

        foreach ($this->reflection()->getMethods($scope) as $method) {
            $methods[] = $method->getName();
        }

        return $methods;
    }

    /**
     * Return an array of properties for the defined scope.
     *
     * @param int $scope
     * @return Vector<string>
     */
    protected function _properties(int $scope): Vector<string> {
        $props = Vector {};

        foreach ($this->reflection()->getProperties($scope) as $prop) {
            $props[] = $prop->getName();
        }

        return $props;
    }

}