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
 * Defines type aliases that are used by the HTTP package.
 */

namespace Titon\Http {
    type HeaderList = Vector<string>;
    type MethodList = Vector<string>;
    type MimeMap = Map<string, string>;
    type StatusCodeMap = Map<int, string>;
}

namespace Titon\Http\Server {
    type RedirectCallback = (function(Response): void);
}
