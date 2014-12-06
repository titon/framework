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

namespace Titon\Io {
    type DomainList = Vector<string>;
    type DomainPathMap = Map<string, PathList>;
    type PathList = Vector<string>;
    type ReaderMap = Map<string, Reader>;
    type ResourceMap<Tk, Tv> = Map<Tk, Tv>;
    type WriterMap = Map<string, Writer>;
}