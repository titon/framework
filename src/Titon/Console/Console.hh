<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Console;

use Titon\Console\Input;
use Titon\Console\Output;
use Titon\Console\InputDefinition\Flag;

/**
 * The `Console` class bootstraps and handles Input and Output to process and
 * run necessary commands.
 *
 * @package Titon\Console
 */
class Console {

    /**
     * The `Command` object to run
     *
     * @var \Titon\Console\Command|null
     */
    protected ?Command $command;

    /**
     * The `Input` object used to retrieve parsed parameters and commands.
     *
     * @var \Titon\Console\Input
     */
    protected Input $input;

    /**
     * The `Output` object used to send response data to the user.
     *
     * @var \Titon\Console\Output
     */
    protected Output $output;

    /**
     * Construct a new `Console` application.
     */
    public function __construct() {
        $this->input = Input::getInstance();
        $this->output = Output::getInstance();
    }

    /**
     * Add a `Command` to the application to be parsed by the `Input`.
     *
     * @param Command $command  The `Command` object to add
     *
     * @return $this
     */
    public function addCommand(Command $command): this {
        $command->setInput($this->input);
        $command->configure();

        $this->input->addCommand($command);

        return $this;
    }

    /**
     * Bootstrap the `Console` application with default parameters and global
     * settings.
     */
    protected function bootstrap(): void {
        $this->input->addFlag((new Flag('help', 'Display this help screen.'))
            ->alias('h'));
        $this->input->addFlag((new Flag('quiet', 'Suppress all output.'))
            ->alias('q'));
        $this->input->addFlag((new Flag('verbose', 'Set the verbosity of the application\'s output.'))
            ->alias('v')
            ->setStackable(true));
    }

    /**
     * Run the `Console` application.
     */
    public function run(): void {
        $this->bootstrap();
        $this->command = $this->input->getActiveCommand();
        if (is_null($this->command)) {
            $this->input->parse();
            $this->renderHelpScreen();
        } else {
            $this->runCommand($this->command);
        }
    }

    /**
     * Register and run the `Command` object.
     *
     * @param Command $command  The `Command` to run
     */
    public function runCommand(Command $command): void {
        $command->registerInput();
        $this->input->parse();

        if (!is_null($flag = $this->input->getFlag('help'))) {
            invariant($flag instanceof Flag, "Must be a Flag.");

            if ($flag->getValue() === 1) {
                $this->renderHelpScreen($command);
            }
        } else {
            $flag = $this->input->getFlag('quiet');

            $verbositySet = false;
            if (!is_null($flag)) {
                invariant($flag instanceof Flag, "Must be a Flag.");

                if (!is_null($verbosity = (int)$flag->getValue())) {
                    $verbositySet = true;
                    $this->output->setVerbosity($verbosity);
                }
            }

            if ($verbositySet === false) {
                $flag = $this->input->getFlag('verbose');

                if (!is_null($flag)) {
                    invariant($flag instanceof Flag, "Must be a Flag.");

                    if (!is_null($verbosity = (int)$flag->getValue())) {
                        $this->output->setVerbosity($verbosity);
                    }
                }
            }

            $command->run();
        }
    }

    /**
     * Render the help screen for the application or the `Command` passed in.
     *
     * @param \Titon\Console\Command|null $command  The `Command` to render usage for
     */
    public function renderHelpScreen(?Command $command = null): void {
        $helpScreen = new HelpScreen($this->input);
        if (!is_null($command)) {
            $helpScreen->setCommand($command);
        }

        $this->output->out($helpScreen->render());
    }
}