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
 * Defines type aliases that are used by the environment package.
 */

namespace Titon\Environment {
    type ImmutableList = Vector<string>;
    type ImmutableVariableMap = ImmMap<string, mixed>;
    type VariableMap = Map<string, mixed>;
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
    use Titon\Environment\Detector;

    /**
     * @see Titon\Environment\Detector::getVariable()
     */
    function env(string $key): string {
        $env = Depository::getInstance()->make('Titon\Environment\Detector');

        invariant($env instanceof Detector, 'Must be an environment Detector.');

        return $env->getVariable($key);
    }

    /**
     * @see Titon\Environment\Detector::is()
     */
    function is_env(string $key): bool {
        $env = Depository::getInstance()->make('Titon\Environment\Detector');

        invariant($env instanceof Detector, 'Must be an environment Detector.');

        return $env->is($key);
    }

}
