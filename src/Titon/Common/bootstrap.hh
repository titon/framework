<?hh
/**
 * @copyright   2010-2014, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

/**
 * --------------------------------------------------------------
 *  Custom Types
 * --------------------------------------------------------------
 *
 * Defines custom types that should be usable in other packages.
 */

namespace Titon\Common {
    type ArgumentList = Vector<mixed>;
    type CacheMap = Map<string, mixed>;
    type DataMap = Map<string, mixed>;
    type OptionMap = Map<string, mixed>;
}