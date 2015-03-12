<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Console;

use Exception;
use Titon\Console\Input;
use Titon\Console\Output;
use Titon\Console\InputDefinition\Flag;
use ReflectionClass;

/**
 * The `Console` class bootstraps and handles Input and Output to process and
 * run necessary commands.
 *
 * @package Titon\Console
 */
class Console {

    /**
     * A decorator banner to `brand` the application.
     *
     * @var string
     */
    protected string $banner = '';

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
     * The name of the application.
     *
     * @var string
     */
    protected string $name = '';

    /**
     * The `Output` object used to send response data to the user.
     *
     * @var \Titon\Console\Output
     */
    protected Output $output;

    /**
     * The version of the application.
     *
     * @var string
     */
    protected string $version = '';

    /**
     * Construct a new `Console` application.
     */
    public function __construct(?Input $input = null, ?Output $output = null) {
        if (is_null($input)) {
            $input = new Input();
        }

        if (is_null($output)) {
            $output = new Output();
        }

        $this->input = $input;
        $this->output = $output;
    }

    /**
     * Add a `Command` to the application to be parsed by the `Input`.
     *
     * @param Command $command  The `Command` object to add
     *
     * @return $this
     */
    public function addCommand(string $command): this {
        $command = new ReflectionClass($command);
        $command = $command->newInstance($this->input, $this->output);

        try {
            $this->input->addCommand($command);
        } catch (Exception $e) {
            $this->output->renderException($e);
            $this->shutdown();
        }

        return $this;
    }

    /**
     * Bootstrap the `Console` application with default parameters and global
     * settings.
     */
    protected function bootstrap(): void {
        /*
         * Add global flags
         */
        $this->input->addFlag((new Flag('help', 'Display this help screen.'))
            ->alias('h'));
        $this->input->addFlag((new Flag('quiet', 'Suppress all output.'))
            ->alias('q'));
        $this->input->addFlag((new Flag('verbose', 'Set the verbosity of the application\'s output.'))
            ->alias('v')
            ->setStackable(true));

        /*
         * Add default styles
         */
        $this->output->setStyle('info', new StyleDefinition('green'));
        $this->output->setStyle('warning', new StyleDefinition('yellow'));
        $this->output->setStyle('error', new StyleDefinition('red'));
        $this->output->setStyle('exception', new StyleDefinition('white', 'red'));
    }

    /**
     * Retrieve the application's banner.
     *
     * @return string
     */
    public function getBanner(): string {
        return $this->banner;
    }

    /**
     * Retrieve the console's `Input` object.
     *
     * @return \Titon\Console\Input
     */
    public function getInput(): Input {
        return $this->input;
    }

    /**
     * Retrieve the application's name.
     *
     * @return string
     */
    public function getName(): string {
        return $this->name;
    }

    /**
     * Retrieve the console's `Output` object.
     *
     * @return \Titon\Console\Output
     */
    public function getOutput(): Output {
        return $this->output;
    }

    /**
     * Retrieve the application's version.
     *
     * @return string
     */
    public function getVersion(): string {
        return $this->version;
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

        $this->shutdown();
    }

    /**
     * Register and run the `Command` object.
     *
     * @param Command $command  The `Command` to run
     */
    public function runCommand(Command $command): void {
        $command->registerInput();
        $this->input->parse();

        $flag = $this->input->getFlag('help');

        if ($flag->getValue() === 1) {
            $this->renderHelpScreen($command);
            return;
        }

        $flag = $this->input->getFlag('quiet');

        $verbositySet = false;

        if ($flag->exists()) {
            $verbositySet = true;
            $this->output->setVerbosity(0);
        }

        if ($verbositySet === false) {
            $flag = $this->input->getFlag('verbose');
            $verbosity = $flag->getValue(1);

            invariant(!is_null($verbosity), "Must not be null.");

            $this->output->setVerbosity($verbosity);
        }

        try {
            $command->run();
        } catch (Exception $e) {
            $this->output->renderException($e);
            $this->shutdown();
        }
    }

    /**
     * Render the help screen for the application or the `Command` passed in.
     *
     * @param \Titon\Console\Command|null $command  The `Command` to render usage for
     */
    public function renderHelpScreen(?Command $command = null): void {
        $helpScreen = new HelpScreen($this);
        if (!is_null($command)) {
            $helpScreen->setCommand($command);
        }

        $this->output->out($helpScreen->render());
    }

    /**
     * Set the banner of the application.
     *
     * @param string $banner    The banner decorator
     *
     * @return $this
     */
    public function setBanner(string $banner): this {
        $this->banner = $banner;

        return $this;
    }

    /**
     * Set the name of the application.
     *
     * @param string $name  The name of the application
     *
     * @return $this
     */
    public function setName(string $name): this {
        $this->name = $name;

        return $this;
    }

    /**
     * Set the version of the application.
     *
     * @param string $version   The version of the application
     *
     * @return $this
     */
    public function setVersion(string $version): this {
        $this->version = $version;

        return $this;
    }

    /**
     * Shutdown method executed at the end of the application's run.
     */
    protected function shutdown(): void {
        exit(1);
    }
}