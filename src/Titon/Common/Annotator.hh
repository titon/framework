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
     * Return a list of all methods that have annotations.
     *
     * @return Vector<string>
     */
    public function getAnnotatedMethods(): Vector<string>;

    /**
     * Return the arguments for a single annotation on the current class.
     *
     * @param string $name
     * @return \Titon\Common\ArgumentList
     */
    public function getClassAnnotation(string $name): ArgumentList;

    /**
     * Return a map of all annotations defined on the current class.
     *
     * @return \Titon\Common\AnnotationMap
     */
    public function getClassAnnotations(): AnnotationMap;

    /**
     * Return the arguments for a single annotation defined on a method.
     *
     * @param string $method
     * @param string $name
     * @return \Titon\Common\ArgumentList
     */
    public function getMethodAnnotation(string $method, string $name): ArgumentList;

    /**
     * Return a map of all annotation defined for a method on the current class.
     *
     * @param string $method
     * @return \Titon\Common\AnnotationMap
     */
    public function getMethodAnnotations(string $method): AnnotationMap;

}