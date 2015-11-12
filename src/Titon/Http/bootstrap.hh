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
    type AcceptHeader = shape('value' => string, 'quality' => float);
    type HeaderList = Set<string>;
    type HeaderMap = Map<string, array<string>>;
    type MetaDataMap = Map<string, mixed>;
    type MethodList = Set<string>;
    type StatusCodeMap = Map<int, string>;
}

namespace Titon\Http\Server {
    type RedirectCallback = (function(Response): void);
}
