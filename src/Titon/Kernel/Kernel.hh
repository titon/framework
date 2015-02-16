<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Kernel;

use Titon\Event\Subject;

interface Kernel extends Subject {

    public function handle(Input $input): Output;

    public function pipe(Middleware $middleware): this;

    public function shutdown(): this;

    public function startup(): this;

    public function terminate(): this;

}
