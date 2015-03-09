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
use Titon\Console\InputDefinition\Argument;
use Titon\Console\InputDefinition\Flag;
use Titon\Console\InputDefinition\Option;
use Titon\Console\Output;

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
    protected string $name;

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
    public function __construct() {
        $this->input = new Input();
        $this->arguments = new InputBag();
        $this->flags = new InputBag();
        $this->options = new InputBag();

        $this->output = Output::getInstance();
    }

    /**
     * Add a new `Argument` to be registered and parsed with the `Input`.
     *
     * @param \Titon\Console\Argument $argument
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
     * @param \Titon\Console\Flag $flag
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
     * @param \Titon\Console\Option $option
     *
     * @return $this
     */
    public function addOption(Option $option): this {
        $this->options->set($option->getName(), $option);

        return $this;
    }

    /**
     * Retrieve an `Argument` value by key.
     *
     * @param string $key
     *
     * @return mixed
     */
    protected function getArgument(string $key): mixed {
        if ($argument = $this->input->getArgument($key)) {
            return $argument->getValue();
        }

        return null;
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
    protected function getFlag(string $key): mixed {
        if ($flag = $this->input->getFlag($key)) {
            return $flag->getValue();
        }

        return null;
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
    protected function getOption(string $key): mixed {
        if ($option = $this->input->getOption($key)) {
            return $option->getValue();
        }

        return null;
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
}