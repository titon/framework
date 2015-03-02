<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Kernel;

use Titon\Event\EmitsEvents;
use Titon\Event\Subject;
use Titon\Kernel\Event\ShutdownEvent;
use Titon\Kernel\Event\StartupEvent;
use Titon\Kernel\Event\TerminateEvent;
use Titon\Kernel\Middleware\Pipeline;

/**
 * The AbstractKernel is the base kernel implementation that handles the middleware pipeline process.
 * All child kernels must implement the `handle()` method to customize the input and output handling process.
 *
 * @package Titon\Kernel
 */
abstract class AbstractKernel<Ti as Input, To as Output> implements Kernel<Ti, To>, Subject {
    use EmitsEvents;

    /**
     * The current application.
     *
     * @var \Titon\Kernel\Application
     */
    protected Application $app;

    /**
     * The contextual input object.
     *
     * @var \Titon\Kernel\Input
     */
    protected ?Ti $input;

    /**
     * The contextual output object.
     *
     * @var \Titon\Kernel\Output
     */
    protected ?To $output;

    /**
     * Pipeline to manage middleware.
     *
     * @var \Titon\Kernel\Middleware\Pipeline
     */
    protected Pipeline<Ti, To> $pipeline;

    /**
     * The time the execution started.
     *
     * @var float
     */
    protected float $startTime;

    /**
     * Instantiate a new application and pipeline.
     *
     * @param \Titon\Kernel\Application $app
     * @param \Titon\Kernel\Middleware\Pipeline $pipeline
     */
    public function __construct(Application $app, Pipeline<Ti, To> $pipeline) {
        $this->app = $app;
        $this->pipeline = $pipeline;
        $this->startTime = microtime(true);
    }

    /**
     * {@inheritdoc}
     */
    public function getApplication(): Application {
        return $this->app;
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
     * {@inheritdoc}
     */
    public function getInput(): ?Ti {
        return $this->input;
    }

    /**
     * {@inheritdoc}
     */
    public function getOutput(): ?To {
        return $this->output;
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
    final public function run(Ti $input, To $output): To {
        $this->input = $input;
        $this->output = $output;

        // Emit startup events
        $this->startup();

        // Since the kernel itself is middleware, add the kernel as the last item in the pipeline.
        // This allows the `handle()` method to be ran after all other middleware.
        $this->pipe($this);

        // Handle the middleware pipeline stack
        $this->output = $output = $this->pipeline->handle($input, $output);

        // Emit shutdown events
        $this->shutdown();

        return $output;
    }

    /**
     * {@inheritdoc}
     */
    public function terminate(): void {
        $input = $this->getInput();
        $output = $this->getOutput();

        invariant($input !== null && $output !== null, 'Input and Output must not be null.');

        $this->emit(new TerminateEvent($this, $input, $output));

        exit(0);
    }

    /**
     * Triggered after the pipeline is handled but before the output is sent.
     */
    protected function shutdown(): void {
        $input = $this->getInput();
        $output = $this->getOutput();

        invariant($input !== null && $output !== null, 'Input and Output must not be null.');

        $this->emit(new ShutdownEvent($this, $input, $output));
    }

    /**
     * Triggered before the pipeline is handled.
     */
    protected function startup(): void {
        $input = $this->getInput();
        $output = $this->getOutput();

        invariant($input !== null && $output !== null, 'Input and Output must not be null.');

        $this->emit(new StartupEvent($this, $input, $output));
    }

}
