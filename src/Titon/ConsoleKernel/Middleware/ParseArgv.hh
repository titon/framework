<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\ConsoleKernel\Middleware;

use Titon\Kernel\Middleware;

class ParseArgv implements Middleware {

    /**
     * {@inheritdoc}
     */
    public function handle<Ti, To>(Ti $input, To $output, Next $next): To {
        // Parse $_SERVER['argv'] and set the values in the input.

        return $output;
    }

}
