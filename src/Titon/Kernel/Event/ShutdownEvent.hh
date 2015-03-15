<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Kernel\Event;

use Titon\Event\Event;
use Titon\Kernel\Application;
use Titon\Kernel\Input;
use Titon\Kernel\Kernel;
use Titon\Kernel\Output;

/**
 * The kernel event that occurs after the kernel processes but before the output is sent.
 *
 * @package Titon\Kernel\Event
 */
class ShutdownEvent<Ta as Application, Ti as Input, To as Output> extends Event {

    /**
     * The input instance.
     *
     * @var \Titon\Kernel\Input
     */
    protected Ti $input;

    /**
     * The kernel instance.
     *
     * @var \Titon\Kernel\Kernel
     */
    protected Kernel<Ta, Ti, To> $kernel;

    /**
     * The output instance.
     *
     * @var \Titon\Kernel\Output
     */
    protected To $output;

    /**
     * Store the event settings.
     *
     * @param \Titon\Kernel\Kernel $kernel
     * @param \Titon\Kernel\Input $input
     * @param \Titon\Kernel\Output $output
     */
    public function __construct(Kernel<Ta, Ti, To> $kernel, Ti $input, To $output) {
        $this->kernel = $kernel;
        $this->input = $input;
        $this->output = $output;

        parent::__construct('kernel.shutdown');
    }

    /**
     * Return the input instance.
     *
     * @return \Titon\Kernel\Input
     */
    public function getInput(): Ti {
        return $this->input;
    }

    /**
     * Return the kernel instance.
     *
     * @return \Titon\Kernel\Kernel
     */
    public function getKernel(): Kernel<Ta, Ti, To> {
        return $this->kernel;
    }

    /**
     * Return the output instance.
     *
     * @return \Titon\Kernel\Output
     */
    public function getOutput(): To {
        return $this->output;
    }

}
