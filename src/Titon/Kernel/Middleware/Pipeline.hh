<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Kernel\Middleware;

use Titon\Kernel\Input;
use Titon\Kernel\Kernel;
use Titon\Kernel\Middleware;
use Titon\Kernel\Output;
use SplQueue;

/**
 * The Pipeline handles the management of middleware through the use of a queue data structure.
 *
 * @package Titon\Kernel\Middleware
 */
class Pipeline<Ti as Input, To as Output> {

    /**
     * Middleware items stored in a queue data structure.
     *
     * @var \SplQueue
     */
    protected SplQueue<Middleware> $pipeline;

    /**
     * Create a new queue to store middleware.
     */
    public function __construct() {
        $this->pipeline = new SplQueue();
    }

    /**
     * Start the pipeline process by executing the first middleware in the queue,
     * and passing a handler for the next middleware as an argument callback.
     *
     * @param \Titon\Kernel\Kernel $kernel
     * @param \Titon\Kernel\Input $input
     * @param \Titon\Kernel\Output $output
     * @return \Titon\Kernel\Output
     */
    public function handle(Kernel<Ti, To> $kernel, Ti $input, To $output): To {

        // Since the kernel itself is middleware, add the kernel as the last item in the queue.
        // This allows its `handle()` method to be ran after all other middleware.
        $this->through($kernel);

        return (new Next($this->pipeline))->handle($input, $output);
    }

    /**
     * Add a middleware to the queue.
     *
     * @param \Titon\Kernel\Middleware $middleware
     * @return $this
     */
    public function through(Middleware $middleware): this {
        $this->pipeline->enqueue($middleware);

        return $this;
    }

}
