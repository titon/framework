<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Common;

use \ReflectionClass;
use \ReflectionMethod;

/**
 * The Annotateable trait implements the methods required by the Annotator interface.
 *
 * @package Titon\Common
 */
trait Annotateable {
    require implements Annotator;

    /**
     * {@inheritdoc}
     */
    public function getClassAnnotation(string $name): AnnotationArgumentList {
        return $this->getClassAnnotations()->get($name) ?: Vector {};
    }

    /**
     * {@inheritdoc}
     */
    <<__Memoize>>
    public function getClassAnnotations(): AnnotationMap {
        return $this->packageAnnotations((new ReflectionClass($this))->getAttributes());
    }

    /**
     * {@inheritdoc}
     */
    public function getMethodAnnotation(string $method, string $name): AnnotationArgumentList {
        return $this->getMethodAnnotations($method)->get($name) ?: Vector {};
    }

    /**
     * {@inheritdoc}
     */
    <<__Memoize>>
    public function getMethodAnnotations(string $method): AnnotationMap {
        return $this->packageAnnotations((new ReflectionMethod($this, $method))->getAttributes());
    }

    /**
     * {@inheritdoc}
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