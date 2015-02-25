<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Kernel;

/**
 * Represents the output or response for the current context.
 *
 * @package Titon\Kernel
 */
interface Output {

    /**
     * Send the output directly to the client.
     */
    public function send(): void;

}
