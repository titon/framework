<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Console;

use Titon\Kernel\AbstractKernel;
use Titon\Kernel\Middleware\Next;

/**
 * The `Console` class acts as the `Kernel` of a CLI application.
 *
 * @package Titon\Console
 */
class Console extends AbstractKernel<Application, Input, Output> {

    /**
     * {@inheritdoc}
     */
    public function handle(Input $input, Output $output, Next<Input, Output> $next): Output {
        $this->exitCode = $this->getApplication()->run($input, $output);

        return $output;
    }
}
