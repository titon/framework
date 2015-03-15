<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Kernel;

/**
 * A Kernel acts as the central point of an application -- it is the application.
 * When ran, it receives an input and an output for the current context,
 * processes it, and in turn sends an output back to the client.
 *
 * @package Titon\Kernel
 */
interface Kernel<Ti as Input, To as Output> extends Middleware<Ti, To> {

    /**
     * Return the current application.
     *
     * @return \Titon\Kernel\Application
     */
    public function getApplication(): Application;

    /**
     * Return the contextual input object.
     *
     * @return \Titon\Kernel\Input
     */
    public function getInput(): ?Ti;

    /**
     * Return the contextual output object.
     *
     * @return \Titon\Kernel\Output
     */
    public function getOutput(): ?To;

    /**
     * Add a middleware to the pipeline. Middleware will be executed in a nested process.
     *
     * @param \Titon\Kernel\Middleware $middleware
     * @return $this
     */
    public function pipe(Middleware<Ti, To> $middleware): this;

    /**
     * Run the startup, pipeline, and shutdown processes.
     * The output must be returned so that output can be sent to the client.
     *
     * @param \Titon\Kernel\Input $input
     * @param \Titon\Kernel\Output $output
     * @return \Titon\Kernel\Output
     */
    public function run(Ti $input, To $output): To;

    /**
     * Terminate the kernel and finalize the process after the output is sent.
     * This should exit the script using `exit()`.
     */
    public function terminate(): void;

}
