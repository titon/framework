<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Common;

use Titon\Common\ArgumentList;
use Titon\Common\Exception\MissingAnnotationException;
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
    <<__Memoize>>
    public function getAnnotatedMethods(): Vector<string> {
        $reflection = new ReflectionClass($this);
        $methods = Vector {};

        foreach ($reflection->getMethods() as $method) {
            if ($this->getMethodAnnotations($method->getName())) {
                $methods[] = $method->getName();
            }
        }

        return $methods;
    }

    /**
     * {@inheritdoc}
     */
    public function getClassAnnotation(string $name): ArgumentList {
        $annotations = $this->getClassAnnotations();

        if ($annotations->contains($name)) {
            return $annotations[$name];
        }

        throw new MissingAnnotationException(sprintf('Class annotation %s does not exist', $name));
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
    public function getMethodAnnotation(string $method, string $name): ArgumentList {
        $annotations = $this->getMethodAnnotations($method);

        if ($annotations->contains($name)) {
            return $annotations[$name];
        }

        throw new MissingAnnotationException(sprintf('Method %s annotation %s does not exist', $method, $name));
    }

    /**
     * {@inheritdoc}
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