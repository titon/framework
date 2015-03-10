<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Console\Command;

use Titon\Console\InputBag;
use Titon\Console\Command;
use Titon\Console\Input;
use Titon\Console\Output;
use Titon\Console\InputDefinition\Argument;
use Titon\Console\InputDefinition\Flag;
use Titon\Console\InputDefinition\Option;
use Titon\Console\UserInput\Confirm;
use Titon\Console\UserInput\Menu;
use Titon\Console\UserInput\Prompt;

/**
 * A `Command` is a class that configures necessary command line inputs from the
 * user and executes its `run` method when called.
 *
 * @package Titon\Console\Command
 */
abstract class AbstractCommand implements Command {

    /**
     * Bag container holding all registered `Argument` objects
     *
     * @var \Titon\Console\InputBag<Argument>
     */
    protected InputBag<Argument> $arguments;

    /**
     * The description of the command used when rendering its help screen.
     *
     * @var string
     */
    protected string $description = '';

    /**
     * Bag container holding all registered `Flag` objects
     *
     * @var \Titon\Console\InputBag<Flag>
     */
    protected InputBag<Flag> $flags;

    /**
     * The `Input` object containing all registered and parsed command line
     * parameters.
     *
     * @var \Titon\Console\Input
     */
    protected Input $input;

    /**
     * The name of the command passed into the command line.
     *
     * @var string
     */
    protected string $name = '';

    /**
     * Bag container holding all registered `Option` objects
     *
     * @var \Titon\Console\InputBag<Option>
     */
    protected InputBag<Option> $options;

    /**
     * The `Output` object to handle output to the user.
     *
     * @var \Titon\Console\Output
     */
    protected Output $output;

    /**
     * Construct a new instance of a command.
     */
    public function __construct(Input $input, Output $output) {
        $this->input = $input;
        $this->output = $output;
        $this->arguments = new InputBag();
        $this->flags = new InputBag();
        $this->options = new InputBag();
    }

    /**
     * Add a new `Argument` to be registered and parsed with the `Input`.
     *
     * @param \Titon\Console\InputDefinition\Argument $argument
     *
     * @return $this
     */
    public function addArgument(Argument $argument): this {
        $this->arguments->set($argument->getName(), $argument);

        return $this;
    }

    /**
     * Add a new `Flag` to be registered and parsed with the `Input`.
     *
     * @param \Titon\Console\InputDefinition\Flag $flag
     *
     * @return $this
     */
    public function addFlag(Flag $flag): this {
        $this->flags->set($flag->getName(), $flag);

        return $this;
    }

    /**
     * Add a new `Option` to be registered and parsed with the `Input`.
     *
     * @param \Titon\Console\InputDefinition\Option $option
     *
     * @return $this
     */
    public function addOption(Option $option): this {
        $this->options->set($option->getName(), $option);

        return $this;
    }

    /**
     * Construct and return a new `Menu` object given the choices and display
     * message.
     *
     * @param Map<mixed, mixed> $choices    Accepted values
     * @param string $message               The message to display before the choices
     *
     * @return \Titon\Console\UserInput\Confirm
     */
    protected function confirm(string $default = ''): Confirm {
        $confirm = new Confirm($this->input, $this->output);
        $confirm->setDefault($default);

        return $confirm;
    }

    /**
     * Alias method for sending output through STDERROR.
     *
     * @param string $output    The message to send
     */
    protected function error(string $output): void {
        $this->output->error($output);
    }

    /**
     * Retrieve an `Argument` value by key.
     *
     * @param string $key
     *
     * @return mixed
     */
    protected function getArgument(string $key, ?string $default = null): ?string {
        return $this->input->getArgument($key)->getValue($default);
    }

    /**
     * Retrieve all `Argument` objects registered specifically to this command.
     *
     * @return \Titon\Console\InputBag<Argument>
     */
    public function getArguments(): InputBag<Argument> {
        return $this->arguments;
    }

    /**
     * Retrieve the command's description.
     *
     * @return string
     */
    public function getDescription(): string {
        return $this->description;
    }

    /**
     * Retrieve a `Flag` value by key.
     *
     * @param string $key
     *
     * @return mixed
     */
    protected function getFlag(string $key, ?int $default = null): ?int {
        return $this->input->getFlag($key)->getValue($default);
    }

    /**
     * Retrieve all `Flag` objects registered specifically to this command.
     *
     * @return \Titon\Console\InputBag<Flag>
     */
    public function getFlags(): InputBag<Flag> {
        return $this->flags;
    }

    /**
     * Retrieve the command's name.
     *
     * @return  string
     */
    public function getName(): string {
        return $this->name;
    }

    /**
     * Retrieve an `Option` value by key.
     *
     * @param string $key
     *
     * @return mixed
     */
    protected function getOption(string $key, ?string $default = null): ?string {
        return $this->input->getOption($key)->getValue($default);
    }

    /**
     * Retrieve all `Option` objects registered specifically to this command.
     *
     * @return \Titon\Console\InputBag<Option>
     */
    public function getOptions(): InputBag<Option> {
        return $this->options;
    }

    /**
     * Construct and return a new `Menu` object given the choices and display
     * message.
     *
     * @param Map<mixed, mixed> $choices    Accepted values
     * @param string $message               The message to display before the choices
     *
     * @return \Titon\Console\UserInput\Menu
     */
    protected function menu(Map<string, string> $choices, string $message = ''): Menu {
        $menu = new Menu($this->input, $this->output);
        $menu->setAcceptedValues($choices)->setMessage($message);

        return $menu;
    }

    /**
     * Alias method for sending output through STDOUT.
     *
     * @param string $output    The message to send
     */
    protected function out(string $output): void {
        $this->output->out($output);
    }

    /**
     * Construct and return a new `Prompt` object given the accepted choices and
     * default value.
     *
     * @param Map<mixed, mixed> $choices    Accepted values
     * @param string $default               Default value
     *
     * @return \Titon\Console\UserInput\Prompt
     */
    protected function prompt(Map<string, string> $choices = Map {}, string $default = ''): Prompt {
        $prompt = new Prompt($this->input, $this->output);
        $prompt->setAcceptedValues($choices)->setDefault($default);

        return $prompt;
    }

    /**
     * {@inheritdoc}
     */
    public function registerInput(): this {
        foreach ($this->arguments as $name => $argument) {
            $this->input->addArgument($argument);
        }
        foreach ($this->flags as $name => $flag) {
            $this->input->addFlag($flag);
        }
        foreach ($this->options as $name => $option) {
            $this->input->addOption($option);
        }

        return $this;
    }

    /**
     * Set the command's description.
     *
     * @param string $description The description for the command
     *
     * @return $this
     */
    public function setDescription(string $description): this {
        $this->description = $description;

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function setInput(Input $input): this {
        $this->input = $input;

        return $this;
    }

    /**
     * Set the command's name.
     *
     * @param string $name The name of the command
     *
     * @return $this
     */
    public function setName(string $name): this {
        $this->name = $name;

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function setOutput(Output $output): this {
        $this->output = $output;

        return $this;
    }
}