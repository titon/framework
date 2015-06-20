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
 * Defines type aliases that are used by the IO package.
 */

namespace Titon\Io {
    type DomainList = Vector<string>;
    type DomainPathMap = Map<string, PathList>;
    type PathList = Set<string>;
    type ReaderList = Vector<Reader>;
    type ReaderMap = Map<string, Reader>;
    type ResourceMap = Map<string, mixed>;
    type WriterMap = Map<string, Writer>;
}
