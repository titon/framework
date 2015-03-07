<?hh
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

/**
 * --------------------------------------------------------------
 *  Custom Types
 * --------------------------------------------------------------
 *
 * Defines type aliases that are used by the annotation package.
 */

namespace Titon\Annotation {
    type AnnotationMap = Map<string, Annotation>;
    type ArgumentList = array<mixed>;
    type MethodAnnotationMap = Map<string, AnnotationMap>;
    type RegistryMap = Map<string, string>;
}
