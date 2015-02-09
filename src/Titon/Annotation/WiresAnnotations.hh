<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Annotation;

use ReflectionClass;

trait WiresAnnotations {

    protected ?Reader $annotationReader;

    protected function wireAnnotations(): this {
        $reader = $this->getAnnotationReader();

        foreach ($reader->getClassAnnotations() as $annotation) {
            if ($annotation instanceof Wireable) {
                $annotation->wire($this);
            }
        }

        foreach ($reader->getAnnotatedMethods() as $method => $annotations) {
            foreach ($annotations as $annotation) {
                if ($annotation instanceof Wireable) {
                    $annotation->wire($this, $method);
                }
            }
        }

        return $this;
    }

    protected function wireClassAnnotation(string $name): Annotation {
        $annotation = $this->getAnnotationReader()->getClassAnnotation($name);

        if ($annotation instanceof Wireable) {
            $annotation->wire($this);
        }

        return $annotation;
    }

    protected function wireMethodAnnotation(string $method, string $name): Annotation {
        $annotation = $this->getAnnotationReader()->getMethodAnnotation($method, $name);

        if ($annotation instanceof Wireable) {
            $annotation->wire($this, $method);
        }

        return $annotation;
    }

    protected function getAnnotationReader(): Reader {
        if ($this->annotationReader) {
            return $this->annotationReader;
        }

        return $this->annotationReader = new Reader(new ReflectionClass($this));
    }

}
