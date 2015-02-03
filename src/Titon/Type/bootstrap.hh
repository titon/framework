<?hh
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

/**
 * --------------------------------------------------------------
 *  Type Aliases
 * --------------------------------------------------------------
 *
 * Defines type aliases that are used by the type package.
 */

namespace Titon\Type\Xml {
    type AttributeMap = Map<string, string>;
    type ElementList = Vector<Element>;
    type NamespaceMap = Map<string, string>;
    type XmlMap = Map<string, mixed>;
}
