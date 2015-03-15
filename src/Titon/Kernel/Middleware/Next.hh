<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Kernel\Middleware;

use Titon\Kernel\Input;
use Titon\Kernel\Middleware;
use Titon\Kernel\Output;
use SplQueue;

/**
 * The Next class handles the nested execution of middleware in the pipeline.
 *
 * @package Titon\Kernel\Middleware
 */
class Next<Ti as Input, To as Output> {

    /**
     * The middleware in the pipeline to execute.
     *
     * @var \SplQueue
     */
    protected SplQueue<Middleware<Ti, To>> $pipeline;

    /**
     * Store the pipeline to handle.
     *
     * @param \SplQueue $pipeline
     */
    public function __construct(SplQueue<Middleware<Ti, To>> $pipeline) {
        $this->pipeline = $pipeline;
    }

    /**
     * This method will dequeue the next middleware in line and execute its `handle()` method.
     * A new `Next` instance should be instantiated so the middleware next in line can be executed.
     *
     * @param \Titon\Kernel\Input $input
     * @param \Titon\Kernel\Output $output
     * @return \Titon\Kernel\Output
     */
    public function handle(Ti $input, To $output): To {
        if (count($this->pipeline) <= 0) {
            return $output;
        }

        $middleware = $this->pipeline->dequeue();
        $handler = inst_meth($middleware, 'handle');

        return $handler($input, $output, new Next($this->pipeline));
    }

}
