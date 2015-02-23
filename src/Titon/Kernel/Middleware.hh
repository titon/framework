<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Kernel;

use Titon\Kernel\Middleware\Next;

/**
 * A Middleware class is used for wrapping a core application with additional functionality.
 * The chain of execution is then handled by the middleware, instead of the application,
 * as the next execution in the stack is manually triggered by a single middleware.
 *
 * @package Titon\Kernel
 */
interface Middleware {

    /**
     * Handle the current input and output and then process the next middleware in the stack.
     *
     * @param \Titon\Kernel\Input $input
     * @param \Titon\Kernel\Output $output
     * @param \Titon\Kernel\Middleware\Next $next
     * @return \Titon\Kernel\Output
     */
    public function handle(Input $input, Output $output, Next $next): Output;

}
