<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Kernel;

use Titon\Kernel\Middleware\Next;

interface Middleware {

    public function handle(Input $input, Output $output, Next $next): Output;

}
