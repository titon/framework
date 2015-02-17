<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Kernel;

interface Middleware {

    public function before(Input $input, Output $output, ?MiddlewareCallback $next = null): void;

    public function after(Input $input, Output $output, ?MiddlewareCallback $next = null): void;

}
