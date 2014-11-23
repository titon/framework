<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Event;

use Titon\Common\Annotator;

/**
 * The ObserverAnnotation provides access to the <<Observer>> annotation which will auto-wire callbacks into an emitter.
 * This annotation requires the Subject interface to be implemented on a class.
 *
 * @package Titon\Event
 */
trait ObserverAnnotation {
    require implements Annotator;
    require implements Subject;

    /**
     * Wire up observers by looking for method annotations. The method in turn will become a callback.
     * The following format is supported.
     *
     *      <<Observer($event[, $priority[, $once]])>>
     */
    private function __wireObserverAnnotations(): void {
        foreach ($this->getAnnotatedMethods() as $method) {
            if ($annotation = $this->getMethodAnnotation($method, 'Observer')) {
                $this->on($annotation[0], inst_meth($this, $method), $annotation->get(1) ?: 0, $annotation->get(2) ?: false);
            }
        }
    }

}