<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Kernel;

// https://mwop.net/blog/2015-01-08-on-http-middleware-and-psr-7.html

use Titon\Context\Depository;
use Titon\Event\EmitsEvents;
use Titon\Event\Subject;
use Titon\Kernel\Middleware\Pipeline;

abstract class AbstractKernel extends Depository implements Kernel, Subject {
    use EmitsEvents;

    protected Pipeline $pipeline;

    public function __construct() {
        $this->pipeline = new Pipeline();
    }

    public function pipe(Middleware $middleware): this {
        $this->pipeline->through($middleware);

        return $this;
    }

    public function run(Input $input, Output $output): Output {
        //$this->emit('kernel.startup', [$input, $output]);

        $output = $this->pipeline->send($this, $input, $output);

        //$this->emit('kernel.shutdown', [$input, $output]);

        return $output;
    }

}
