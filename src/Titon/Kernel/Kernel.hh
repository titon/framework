<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Kernel;

interface Kernel extends Middleware {

    public function pipe(Middleware $middleware): this;

    public function run(Input $input, Output $output): Output;

    //public function terminate(): this;

}
