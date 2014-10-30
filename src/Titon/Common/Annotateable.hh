<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Common;

use \ReflectionClass;
use \ReflectionMethod;

type AnnotationArgumentList = Vector<mixed>;
type AnnotationMap = Map<string, AnnotationArgumentList>;

/**
 * The Annotateable trait provides an easy access layer to annotations (also known as attributes)
 * defined on the current class and its methods.
 *
 * @package Titon\Common
 */
trait Annotateable {

    /**
     * Return the arguments for a single attribute on the current class.
     *
     * @param string $name
     * @return \Titon\Common\AnnotationArgumentList
     */
    public function getClassAnnotation(string $name): AnnotationArgumentList {
        return $this->getClassAnnotations()->get($name) ?: Vector {};
    }

    /**
     * Return a map of all attributes defined on the current class.
     *
     * @return \Titon\Common\AnnotationMap
     */
    <<__Memoize>>
    public function getClassAnnotations(): AnnotationMap {
        return $this->packageAnnotations((new ReflectionClass($this))->getAttributes());
    }

    /**
     * Return the arguments for a single attribute defined on a method.
     *
     * @param string $method
     * @param string $name
     * @return \Titon\Common\AnnotationArgumentList
     */
    public function getMethodAnnotation(string $method, string $name): AnnotationArgumentList {
        return $this->getMethodAnnotations($method)->get($name) ?: Vector {};
    }

    /**
     * Return a map of all attributes defined for a method on the current class.
     *
     * @param string $method
     * @return \Titon\Common\AnnotationMap
     */
    <<__Memoize>>
    public function getMethodAnnotations(string $method): AnnotationMap {
        return $this->packageAnnotations((new ReflectionMethod($this, $method))->getAttributes());
    }

    /**
     * Convert the attributes returned from the reflection layer into maps and vectors.
     *
     * @param array<string, array<mixed>> $attributes
     * @return \Titon\Common\AnnotationMap
     */
    public function packageAnnotations(array<string, array<mixed>> $attributes): AnnotationMap {
        $annotations = Map {};

        foreach ($attributes as $name => $values) {
            $arguments = Vector {};

            foreach ($values as $value) {
                if (is_array($value)) {
                    $arguments[] = new Map($value);
                } else {
                    $arguments[] = $value;
                }
            }

            $annotations[$name] = $arguments;
        }

        return $annotations;
    }

}