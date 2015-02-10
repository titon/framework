<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Annotation;

use Titon\Annotation\Exception\MissingAnnotationException;
use ReflectionClass;

/**
 * The Reader class provides a straight forward approach for reading class and method annotations.
 * Each annotation that is read is converted to a mapped annotation class.
 *
 * @package Titon\Annotation
 */
class Reader {

    /**
     * The class reflection to read annotations from.
     *
     * @var \ReflectionClass
     */
    protected ReflectionClass $class;

    /**
     * Instantiate a new class reflection layer based on the specified class.
     *
     * @param mixed $class
     */
    public function __construct(mixed $class) {
        $this->class = new ReflectionClass($class);
    }

    /**
     * Return a map of all methods that have annotations.
     *
     * @return \Titon\Annotation\MethodAnnotationMap
     */
    <<__Memoize>>
    public function getAnnotatedMethods(): MethodAnnotationMap {
        $methods = Map {};

        foreach ($this->class->getMethods() as $method) {
            if ($annotations = $this->getMethodAnnotations($method->getName())) {
                $methods[$method->getName()] = $annotations;
            }
        }

        return $methods;
    }

    /**
     * Return the annotation instance defined by name.
     * If no annotation exists, throw an exception.
     *
     * @param string $name
     * @return \Titon\Annotation\Annotation
     * @throws \Titon\Annotation\Exception\MissingAnnotationException
     */
    public function getClassAnnotation(string $name): Annotation {
        $annotations = $this->getClassAnnotations();

        if ($annotations->contains($name)) {
            return $annotations[$name];
        }

        throw new MissingAnnotationException(sprintf('Class annotation %s does not exist', $name));
    }

    /**
     * Return a map of all annotations defined on the current class.
     *
     * @return \Titon\Annotation\AnnotationMap
     */
    <<__Memoize>>
    public function getClassAnnotations(): AnnotationMap {
        return $this->packageAnnotations($this->class->getAttributes());
    }

    /**
     * Return the annotation instance defined by name on a specific method.
     * If no annotation exists, throw an exception.
     *
     * @param string $method
     * @param string $name
     * @return \Titon\Annotation\Annotation
     * @throws \Titon\Annotation\Exception\MissingAnnotationException
     */
    public function getMethodAnnotation(string $method, string $name): Annotation {
        $annotations = $this->getMethodAnnotations($method);

        if ($annotations->contains($name)) {
            return $annotations[$name];
        }

        throw new MissingAnnotationException(sprintf('Method %s annotation %s does not exist', $method, $name));
    }

    /**
     * Return a map of all annotations defined on a method on the current class.
     *
     * @param string $method
     * @return \Titon\Annotation\AnnotationMap
     */
    <<__Memoize>>
    public function getMethodAnnotations(string $method): AnnotationMap  {
        return $this->packageAnnotations($this->class->getMethod($method)->getAttributes());
    }

    /**
     * Convert the attributes returned from the reflection layer into annotation classes.
     *
     * @param array<string, array<mixed>> $attributes
     * @return \Titon\Annotation\AnnotationMap
     */
    protected function packageAnnotations(array<string, array<mixed>> $attributes): AnnotationMap {
        $annotations = Map {};

        foreach ($attributes as $name => $arguments) {
            if (substr($name, 0, 2) === '__') {
                continue; // Ignore built-ins
            }

            $annotations[$name] = Registry::factory($name, $arguments);
        }

        return $annotations;
    }

}
