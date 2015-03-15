<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Console;

use Titon\Kernel\AbstractKernel;
use Titon\Kernel\Middleware\Next;

class Kernel extends AbstractKernel<Console, Input, Output> {
    public function handle(Input $input, Output $output, Next<Input, Output> $next): Output {
        $input->stack[] = 'kernel';
        $output->ran = true;

        $this->exitCode = $this->getApplication()->run($input, $output);

        // UNSAFE
        return $output;
    }
}