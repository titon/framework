<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Console;

use Titon\Console\InputDefinition\AbstractInputDefinition;
use Titon\Utility\State\Server;
use Titon\Console\InputDefinition\Argument;
use Titon\Console\InputDefinition\Flag;
use Titon\Console\InputDefinition\Option;
use Titon\Console\Exception\MissingValueException;
use Titon\Console\Exception\InvalidNumberOfCommandsException;
use Titon\Console\Exception\InvalidNumberOfArgumentsException;
use Titon\Console\Exception\InvalidInputDefinitionException;

/**
 * The `Input` class contains all available `Flag`, `Argument`, `Option`, and
 * `Command` objects available to parse given the provided input.
 *
 * @package Titon\Console
 */
class Input {

    const string STREAM_STDIN = 'php://stdin';

    /**
     * Bag container holding all registered `Argument` objects
     *
     * @var \Titon\Console\InputBag<Argument>
     */
    protected InputBag<Argument> $arguments;

    /**
     * The `Command` (if any) that is parsed from the provided input.
     *
     * @var \Titon\Console\Command
     */
    protected ?Command $command;

    /**
     * All available `Command` candidates to parse from the input.
     *
     * @var \Titon\Console\CommandMap
     */
    protected CommandMap $commands = Map {};

    /**
     * Bag container holding all registered `Flag` objects
     *
     * @var \Titon\Console\InputBag<Flag>
     */
    protected InputBag<Flag> $flags;

    /**
     * The `InputLexer` that will traverse and help parse the provided input.
     *
     * @var \Titon\Console\InputLexer
     */
    protected InputLexer $input;

    /**
     * The singleton instance.
     *
     * @var \Titon\Console\Input|null
     */
    protected static ?Input $instance;

    /**
     * All parameters provided in the input that do not match a given `Command`
     * or `InputDefinition`.
     *
     * @var Vector<\Titon\Console\RawInput>
     */
    protected Vector<RawInput> $invalid = Vector {};

    /**
     * Bag container holding all registered `Option` objects
     *
     * @var \Titon\Console\InputBag<Option>
     */
    protected InputBag<Option> $options;

    /**
     * Boolean if the provided input has already been parsed or not.
     *
     * @var bool
     */
    protected bool $parsed = false;

    /**
     * Raw input used at creation of the `Input` object.
     *
     * @var Vector<string>
     */
    protected Vector<string> $rawInput;

    /**
     * Stream handle for user input.
     *
     * @var resource
     */
    protected resource $stdin;

    /**
     * The 'strict' value of the `Input` object. If set to `true`, then any invalid
     * parameters found in the input will throw an exception.
     *
     * @var bool
     */
    protected bool $strict = false;

    /**
     * Construct a new instance of Input
     *
     * @param Vector<string>|null $args
     */
    public function __construct(?Vector<string> $args = null, bool $strict = false) {
        if (is_null($args)) {
            $args = new Vector(array_slice(Server::get('argv'), 1));
        }

        $this->rawInput = $args;
        $this->input = new InputLexer($args);
        $this->flags = new InputBag();
        $this->options = new InputBag();
        $this->arguments = new InputBag();
        $this->strict = $strict;
        $this->stdin = fopen(Input::STREAM_STDIN, 'r');
    }

    /**
     * Add a new `Argument` candidate to be parsed from input.
     *
     * @param \Titon\Console\Argument $argument The `Argument` to add
     *
     * @return $this
     */
    public function addArgument(Argument $argument): this {
        $this->arguments->set($argument->getName(), $argument);

        return $this;
    }

    /**
     * Add a new `Command` candidate to be parsed from input.
     *
     * @param \Titon\Console\Command $command   The `Command` to add
     *
     * @return $this
     */
    public function addCommand(Command $command): this {
        $command->configure();
        $this->commands[$command->getName()] = $command;

        return $this;
    }

    /**
     * Add a new `Flag` candidate to be parsed from input.
     *
     * @param \Titon\Console\Flag $flag The `Flag` to add
     *
     * @return $this
     */
    public function addFlag(Flag $flag): this {
        $this->flags->set($flag->getName(), $flag);

        return $this;
    }

    /**
     * Add a new `Option` candidate to be parsed from input.
     *
     * @param \Titon\Console\Option $option The `Option` to add
     *
     * @return $this
     */
    public function addOption(Option $option): this {
        $this->options->set($option->getName(), $option);

        return $this;
    }

    /**
     * Parse and retrieve the active command from the raw input.
     *
     * @return \Titon\Console\Command|null
     */
    public function getActiveCommand(): ?Command {
        if ($this->parsed === true) {
            return $this->command;
        }

        if (!is_null($this->command)) {
            return $this->command;
        }

        $input = new Vector($this->rawInput);
        foreach ($input as $index => $value) {
            if (!is_null($command = $this->commands->get($value))) {
                $input->removeKey($index);
                $this->setInput($input);
                $this->command = $command;

                return $this->command;
            }
        }

        return null;
    }

    /**
     * Retrieve an `Argument` by its key or alias. Returns null if none exists.
     *
     * @param string $key The key or alias of the `Argument`
     *
     * @return \Titon\Console\Argument
     */
    public function getArgument(string $key): Argument {
        if (is_null($argument = $this->arguments->get($key))) {
            throw new InvalidInputDefinitionException(sprintf("The argument %s doesn't exist.", $key));
        }

        invariant($argument instanceof Argument, "Must be an `Argument`.");

        return $argument;
    }

    /**
     * Retrieve all `Argument` candidates.
     *
     * @return \Titon\Console\InputBag<Argument>
     */
    public function getArguments(): InputBag<Argument> {
        return $this->arguments;
    }

    /**
     * Retrieve a `Command` candidate by its name.
     *
     * @param string $key   The name of the `Command`
     *
     * @return \Titon\Console\Command|null
     */
    public function getCommand(string $name): ?Command {
        return $this->commands->get($name);
    }

    /**
     * Retrieve all `Command` candidates.
     *
     * @return \Titon\Console\CommandMap
     */
    public function getCommands(): CommandMap {
        return $this->commands;
    }

    /**
     * Retrieve a `Flag` by its key or alias. Returns null if none exists.
     *
     * @param string $key The key or alias of the `Flag`
     *
     * @return \Titon\Console\Flag
     */
    public function getFlag(string $key): Flag {
        if (is_null($flag = $this->flags->get($key))) {
            throw new InvalidInputDefinitionException(sprintf("The flag %s doesn't exist.", $key));
        }

        invariant($flag instanceof Flag, "Must be a `Flag`.");

        return $flag;
    }

    /**
     * Retrieve all `Flag` candidates.
     *
     * @return \Titon\Console\InputBag<Flag>
     */
    public function getFlags(): InputBag<Flag> {
        return $this->flags;
    }

    /**
     * Create and return the singleton instance.
     *
     * @return \Titon\Console\Input
     */
    public static function getInstance(): Input {
        if (is_null(self::$instance)) {
            self::$instance = new Input();
        }

        return self::$instance;
    }

    /**
     * Retrieve an `Option` by its key or alias. Returns null if none exists.
     *
     * @param string $key The key or alias of the `Option`
     *
     * @return \Titon\Console\Option
     */
    public function getOption(string $key): Option {
        if (is_null($option = $this->options->get($key))) {
            throw new InvalidInputDefinitionException(sprintf("The option %s doesn't exist.", $key));
        }

        invariant($option instanceof Option, "Must be an `Option`.");

        return $option;
    }

    /**
     * Retrieve all `Option` candidates.
     *
     * @return \Titon\Console\InputBag<Option>
     */
    public function getOptions(): InputBag<Option> {
        return $this->options;
    }

    /**
     * Return whether the `Input` is running in `strict` mode or not.
     *
     * @return bool
     */
    public function getStrict(): bool {
        return $this->strict;
    }

    /**
     * Read in and return input from the user.
     *
     * @return string
     */
    public function getUserInput(): string {
        return trim(fgets($this->stdin));
    }

    /**
     * Parse input for all `Flag`, `Option`, and `Argument` candidates.
     *
     * @return void
     */
    public function parse(): void {
        foreach ($this->input as $val) {
            if ($this->parseFlag($val)) {
                continue;
            }
            if ($this->parseOption($val)) {
                continue;
            }

            if (is_null($this->command)) {
                // If we haven't parsed a command yet, see if we have a match.
                if (!is_null($command = $this->commands->get($val['value']))) {
                    $this->command = $command;
                    continue;
                }
            } else if (!is_null($this->commands->get($val['value']))) {
                if ($this->strict === true) {
                    throw new InvalidNumberOfCommandsException("Multiple commands are not supported.");
                }
            }

            if ($this->parseArgument($val)) {
                continue;
            }

            if ($this->strict === true) {
                throw new InvalidNumberOfArgumentsException(sprintf("No parameter registered for value %s", $val['value']));
            }

            $this->invalid[] = $val;
        }

        if (is_null($this->command) && $this->strict === true) {
            throw new InvalidNumberOfCommandsException("No command was parsed from the input.");
        }

        foreach ($this->flags as $name => $flag) {
            if ($flag->getMode() !== AbstractInputDefinition::MODE_REQUIRED) {
                continue;
            }

            if (is_null($flag->getValue())) {
                throw new MissingValueException(sprintf("Required flag `%s` is not present.", $flag->getName()));
            }
        }

        foreach ($this->options as $name => $option) {
            if ($option->getMode() !== AbstractInputDefinition::MODE_REQUIRED) {
                continue;
            }

            if (is_null($option->getValue())) {
                throw new MissingValueException(sprintf("No value present for required option %s", $option->getName()));
            }
        }

        foreach ($this->arguments as $name => $argument) {
            if ($argument->getMode() !== AbstractInputDefinition::MODE_REQUIRED) {
                continue;
            }

            if (is_null($argument->getValue())) {
                throw new MissingValueException(sprintf("No value present for required argument %s", $argument->getName()));
            }
        }

        $this->parsed = true;
    }

    /**
     * Determine if a RawInput matches an `Argument` candidate. If so, save its
     * value.
     *
     * @param \Titon\Console\RawInput $key
     *
     * @return bool
     */
    protected function parseArgument(RawInput $input): bool {
        foreach ($this->arguments as $argument) {
            if (is_null($argument->getValue())) {
                $argument->setValue($input['raw']);
                $argument->setExists(true);

                return true;
            }
        }

        return false;
    }

    /**
     * Determine if a RawInput matches a `Flag` candidate. If so, save its
     * value.
     *
     * @param \Titon\Console\RawInput $key
     *
     * @return bool
     */
    protected function parseFlag(RawInput $input): bool {
        $key = $input['value'];
        if (!is_null($flag = $this->flags->get($key))) {
            invariant($flag instanceof Flag, "Must be a Flag.");

            if ($flag->isStackable()) {
                $flag->increaseValue();
            } else {
                $flag->setValue(1);
            }

            $flag->setExists(true);

            return true;
        }

        foreach ($this->flags as $name => $flag) {
            if ($key === $flag->getNegativeAlias()) {
                invariant($flag instanceof Flag, "Must be a Flag.");

                $flag->setValue(0);
                $flag->setExists(true);

                return true;
            }
        }

        return false;
    }

    /**
     * Determine if a RawInput matches an `Option` candidate. If so, save its
     * value.
     *
     * @param \Titon\Console\RawInput $key
     *
     * @return bool
     */
    protected function parseOption(RawInput $input): bool {
        $key = $input['value'];
        if (is_null($option = $this->options->get($key))) {
            return false;
        }

        invariant(!is_null($option), "Must not be null.");

        // Peak ahead to make sure we get a value.
        $nextValue = $this->input->peek();

        invariant(!is_null($nextValue), "Must be RawInput.");

        if (!$this->input->end() && $this->input->isArgument($nextValue['raw'])) {
            throw new MissingValueException("No value is present for option $key");
        }

        $this->input->shift();
        $value = $this->input->current();

        $matches = [];
        if (preg_match('#\A"(.+)"$#', $value['raw'], $matches)) {
            $value = $matches[1];
        } else if (preg_match("#\A'(.+)'$#", $value['raw'], $matches)) {
            $value = $matches[1];
        } else {
            $value = $value['raw'];
        }

        $option->setValue($value);
        $option->setExists(true);

        return true;
    }

    /**
     * Set the input to be parsed.
     *
     * @param Vector<string> $args   The input to be parsed
     *
     * @return $this
     */
    public function setInput(Vector<string> $args): this {
        $this->rawInput = $args;
        $this->input = new InputLexer($args);
        $this->parsed = false;
        $this->command = null;

        return $this;
    }

    /**
     * Set the strict value of the `Input`
     *
     * @param bool $strict  Boolean if the `Input` should run strictly or not
     *
     * @return $this
     */
    public function setStrict(bool $strict): this {
        $this->strict = $strict;

        return $this;
    }
}
