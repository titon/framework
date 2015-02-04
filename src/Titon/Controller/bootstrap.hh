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
 * Defines type aliases that are used by the controller package.
 */

namespace Titon\Controller {
    use Titon\Common\ArgumentList;

    type ActionMap = Map<string, ArgumentList>;
}
