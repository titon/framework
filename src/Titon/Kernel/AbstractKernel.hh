<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Kernel;

// https://mwop.net/blog/2015-01-08-on-http-middleware-and-psr-7.html

use Titon\Event\EmitsEvents;
use Titon\Kernel\Middleware\Pipeline;

abstract class AbstractKernel implements Kernel {
    use EmitsEvents;

    protected Pipeline $pipeline;

    public function __construct() {
        $this->pipeline = new Pipeline();
    }

    public function pipe(Middleware $middleware): this {
        $this->pipeline->through($middleware);

        return $this;
    }

    protected function start(Input $input, Output $ouput): void {
        $this->emit('kernel.starting', [$input, $output]);

        $this->pipeline->runBefore($input, $output);

        $this->emit('kernel.started', [$input, $output]);
    }

    protected function stop(Input $input, Output $ouput): void {
        $this->emit('kernel.stopping', [$input, $output]);

        $this->pipeline->runAfter($input, $output);

        $this->emit('kernel.stopped', [$input, $output]);
    }

}
