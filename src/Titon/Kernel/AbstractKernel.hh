<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Kernel;

use Titon\Context\Depository;
use Titon\Event\EmitsEvents;
use Titon\Event\Subject;
use Titon\Kernel\Middleware\Pipeline;

/**
 * The AbstractKernel is the base kernel implementation that handles the middleware pipeline process.
 * All child kernels must implement the `handle()` method to customize the input and output handling process.
 *
 * @package Titon\Kernel
 * @events
 *      kernel.startup(Kernel $kernel, Input $input, Output $output)
 *      kernel.shutdown(Kernel $kernel, Input $input, Output $output)
 */
abstract class AbstractKernel<Ti, To> extends Depository implements Kernel<Ti, To>, Subject {
    use EmitsEvents;

    /**
     * Pipeline to manage middleware.
     *
     * @var \Titon\Kernel\Middleware\Pipeline
     */
    protected Pipeline $pipeline;

    /**
     * The time the execution started.
     *
     * @var float
     */
    protected float $startTime;

    /**
     * Instantiate a new pipeline.
     */
    public function __construct() {
        $this->pipeline = new Pipeline();
        $this->startTime = microtime(true);
    }

    /**
     * Alias method for `pipe()`.
     *
     * @param \Titon\Kernel\Middleware $middleware
     * @return $this
     */
    public function addMiddleware(Middleware $middleware): this {
        return $this->pipe($middleware);
    }

    /**
     * Return the total time it took to execute up to this point.
     *
     * @return float
     */
    public function getExecutionTime(): float {
        return microtime(true) - $this->getStartTime();
    }

    /**
     * Return the execution start time.
     *
     * @return float
     */
    public function getStartTime(): float {
        return $this->startTime;
    }

    /**
     * {@inheritdoc}
     */
    public function pipe(Middleware $middleware): this {
        $this->pipeline->through($middleware);

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function run(Ti $input, To $output): To {
        $this->startup($input, $output);

        $output = $this->pipeline->handle($this, $input, $output);

        $this->shutdown($input, $output);

        return $output;
    }

    /**
     * Triggered after the pipeline is handled and emits the shutdown event.
     *
     * @param \Titon\Kernel\Input $input
     * @param \Titon\Kernel\Output $output
     */
    protected function shutdown(Ti $input, To $output): void {
        $this->emit('kernel.shutdown', [$this, $input, $output]);
    }

    /**
     * Triggered before the pipeline is handled and emits the startup event.
     *
     * @param \Titon\Kernel\Input $input
     * @param \Titon\Kernel\Output $output
     */
    protected function startup(Ti $input, To $output): void {
        $this->emit('kernel.startup', [$this, $input, $output]);
    }

}
