<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Annotation;

/**
 * The WiresAnnotations trait allows a class to wire class level and method level annotations.
 * If an annotation implements the `Titon\Annotation\Wireable` interface, than any wired
 * annotation will be bootstrapped.
 *
 * @package Titon\Annotation
 */
trait WiresAnnotations {

    /**
     * Annotation reader instance.
     *
     * @var \Titon\Annotation\Reader
     */
    protected ?Reader $annotationReader;

    /**
     * Map of wired annotations.
     *
     * @var Map<string, int>
     */
    protected Map<string, int> $wiredAnnotations = Map {};

    /**
     * Return an annotation reader instance. If one does not exist, it will be created.
     *
     * @return \Titon\Annotation\Reader
     */
    protected function getAnnotationReader(): Reader {
        if ($this->annotationReader) {
            return $this->annotationReader;
        }

        return $this->annotationReader = new Reader($this);
    }

    /**
     * Wire all class and method annotations.
     *
     * @return $this
     */
    protected function wireAnnotations(): this {
        $this->wireClassAnnotations();

        foreach ($reader->getAnnotatedMethods() as $method => $annotations) {
            foreach ($annotations as $annotation) {
                $this->wireUp($annotation, $method);
            }
        }

        return $this;
    }

    /**
     * Wire a single class annotation by name and return the annotation.
     *
     * @param string $name
     * @return \Titon\Annotation\Annotation
     */
    protected function wireClassAnnotation(string $name): Annotation {
        return $this->wireUp($this->getAnnotationReader()->getClassAnnotation($name));
    }

    /**
     * Wire all class annotations.
     *
     * @return $this;
     */
    protected function wireClassAnnotations(): this {
        foreach ($this->getAnnotationReader()->getClassAnnotations() as $annotation) {
            $this->wireUp($annotation);
        }

        return $this;
    }

    /**
     * Wire a single method annotation by name and return the annotation.
     *
     * @param string $name
     * @return \Titon\Annotation\Annotation
     */
    protected function wireMethodAnnotation(string $method, string $name): Annotation {
        return $this->wireUp($this->getAnnotationReader()->getMethodAnnotation($method, $name), $method);
    }

    /**
     * Wire all annotations for a specific method.
     *
     * @return $this;
     */
    protected function wireMethodAnnotations(string $method): this {
        foreach ($this->getAnnotationReader()->getMethodAnnotations($method) as $annotation) {
            $this->wireUp($annotation);
        }

        return $this;
    }

    /**
     * Wire up an annotation if it has not been previously wired.
     *
     * @param \Titon\Annotation\Annotation $annotation
     * @param string $method
     * @return \Titon\Annotation\Annotation
     */
    protected function wireUp(Annotation $annotation, string $method = ''): Annotation {
        $key = static::class . ':' . $method . ':' . $annotation->getName();

        if ($this->wiredAnnotations->contains($key)) {
            return $annotation;
        }

        if ($annotation instanceof Wireable) {
            $annotation->wire($this, $method);
        }

        $this->wiredAnnotations[$key] = time();

        // The type checker balks here and thinks we are `Wireable` instead of `Annotation`, so reset it.
        invariant($annotation instanceof Annotation, 'Must be an annotation.');

        return $annotation;
    }

}
