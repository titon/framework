<?hh
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

use Titon\Environment\Environment;

/**
 * --------------------------------------------------------------
 *  Helper Functions
 * --------------------------------------------------------------
 *
 * Defines global helper functions for common use cases.
 */

/**
 * @see Titon\Environment\Environment::getVariable()
 */
function env(string $key): string {
    return Environment::registry()->getVariable($key);
}