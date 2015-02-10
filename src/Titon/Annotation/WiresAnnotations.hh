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
     * @var Map<string, string>
     */
    protected Map<string, string> $wiredAnnotations = Map {};

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
        $reader = $this->getAnnotationReader();

        foreach ($reader->getClassAnnotations() as $annotation) {
            $this->wireUp($annotation);
        }

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
     * Wire a single method annotation by name and return the annotation.
     *
     * @param string $name
     * @return \Titon\Annotation\Annotation
     */
    protected function wireMethodAnnotation(string $method, string $name): Annotation {
        return $this->wireUp($this->getAnnotationReader()->getMethodAnnotation($method, $name), $method);
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

        return $annotation;
    }

}
