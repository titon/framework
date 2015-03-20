<?hh
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

use Titon\Environment\Environment;

/**
 * --------------------------------------------------------------
 *  Type Aliases
 * --------------------------------------------------------------
 *
 * Defines type aliases that are used by the environment package.
 */

namespace Titon\Environment {
    type VariableMap = Map<string, string>;
}

/**
 * --------------------------------------------------------------
 *  Helper Functions
 * --------------------------------------------------------------
 *
 * Defines global helper functions for common use cases.
 */

namespace {
    use Titon\Context\Depository;
    use Titon\Environment\Environment;

    /**
     * @see Titon\Environment\Environment::getVariable()
     */
    function env(string $key): string {
        $env = Depository::getInstance()->make('Titon\Environment\Environment');

        invariant($env instanceof Environment, 'Must be an Environment.');

        return $env->getVariable($key);
    }
}
