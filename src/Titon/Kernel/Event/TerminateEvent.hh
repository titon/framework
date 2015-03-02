<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Kernel\Event;

use Titon\Event\Event;
use Titon\Kernel\Input;
use Titon\Kernel\Kernel;
use Titon\Kernel\Output;

/**
 * The kernel event that occurs when the kernel terminates,
 * which usually triggers after the output has been sent.
 *
 * @package Titon\Kernel\Event
 */
class TerminateEvent extends Event {

    /**
     * The input instance.
     *
     * @var \Titon\Kernel\Input
     */
    protected Input $input;

    /**
     * The kernel instance.
     *
     * @var \Titon\Kernel\Kernel
     */
    protected Kernel $kernel;

    /**
     * The output instance.
     *
     * @var \Titon\Kernel\Output
     */
    protected Output $output;

    /**
     * Store the event settings.
     *
     * @param \Titon\Kernel\Kernel $kernel
     * @param \Titon\Kernel\Input $input
     * @param \Titon\Kernel\Output $output
     */
    public function __construct(Kernel $kernel, Input $input, Output $output) {
        $this->kernel = $kernel;
        $this->input = $input;
        $this->output = $output;

        parent::__construct('kernel.terminate');
    }

    /**
     * Return the input instance.
     *
     * @return \Titon\Kernel\Input
     */
    public function getInput(): Input {
        return $this->input;
    }

    /**
     * Return the kernel instance.
     *
     * @return \Titon\Kernel\Kernel
     */
    public function getKernel(): Kernel {
        return $this->kernel;
    }

    /**
     * Return the output instance.
     *
     * @return \Titon\Kernel\Output
     */
    public function getOutput(): Output {
        return $this->output;
    }

}
