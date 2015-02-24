<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\ConsoleKernel\Middleware;

use Titon\Kernel\Middleware;

class DetectEnvironment implements Middleware {

    /**
     * {@inheritdoc}
     */
    public function handle<Ti, To>(Ti $input, To $output, Next $next): To {
        // Detect environment based on --env or the APP_ENV variable.

        return $output;
    }

}
