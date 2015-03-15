<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Console;

use Titon\Kernel\AbstractKernel;

class Kernel extends AbstractKernel<Input, Output> {

    public function handle(): void {
        $this->getApplication()->run();

        return $this->getApplication()->getOutput();
    }
}