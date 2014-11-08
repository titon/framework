<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Common;

type AnnotationMap = Map<string, ArgumentList>;

/**
 * The Annotator interface defines an easy access layer to annotations (also known as attributes)
 * defined on the current class and its methods.
 *
 * @package Titon\Common
 */
interface Annotator {

    /**
     * Return the arguments for a single attribute on the current class.
     *
     * @param string $name
     * @return \Titon\Common\ArgumentList
     */
    public function getClassAnnotation(string $name): ArgumentList;

    /**
     * Return a map of all attributes defined on the current class.
     *
     * @return \Titon\Common\AnnotationMap
     */
    public function getClassAnnotations(): AnnotationMap;

    /**
     * Return the arguments for a single attribute defined on a method.
     *
     * @param string $method
     * @param string $name
     * @return \Titon\Common\ArgumentList
     */
    public function getMethodAnnotation(string $method, string $name): ArgumentList;

    /**
     * Return a map of all attributes defined for a method on the current class.
     *
     * @param string $method
     * @return \Titon\Common\AnnotationMap
     */
    public function getMethodAnnotations(string $method): AnnotationMap;

    /**
     * Convert the attributes returned from the reflection layer into maps and vectors.
     *
     * @param array<string, array<mixed>> $attributes
     * @return \Titon\Common\AnnotationMap
     */
    public function packageAnnotations(array<string, array<mixed>> $attributes): AnnotationMap;

}