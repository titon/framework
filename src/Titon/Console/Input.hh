<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Console;

use Titon\Common\ArgumentList;
use Titon\Utility\State\Server;
use Titon\Console\InputDefinition\Argument;
use Titon\Console\InputDefinition\Flag;
use Titon\Console\InputDefinition\Option;
use Titon\Console\Exception\MissingValueException;
use Titon\Console\Exception\InvalidCommandException;

/**
 * The `Input` class contains all available `Flag`, `Argument`, `Option`, and
 * `Command` objects available to parse given the provided input.
 *
 * @package Titon\Console
 */
class Input {

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
     * The 'strict' value of the `Input` object. If set to `true`, then any invalid
     * parameters found in the input will throw an exception.
     *
     * @var bool
     */
    protected bool $strict = false;

    /**
     * Construct a new instance of Input
     *
     * @param ?array<mixed> $args
     */
    public function __construct(?array<string> $args = null, bool $strict = false) {
        if (is_null($args)) {
            $args = array_slice(Server::get('argv'), 1);
        }

        $this->input = new InputLexer($args);
        $this->flags = new InputBag();
        $this->options = new InputBag();
        $this->arguments = new InputBag();
        $this->strict = $strict;
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
     * Retrieve an `Argument` by its key or alias. Returns null if none exists.
     *
     * @param string $key The key or alias of the `Argument`
     *
     * @return \Titon\Console\Argument|null
     */
    public function getArgument(string $key): ?Argument {
        return $this->arguments->get($key);
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
     * @return \Titon\Console\Flag|null
     */
    public function getFlag(string $key): ?Flag {
        return $this->flags->get($key);
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
     * Retrieve an `Option` by its key or alias. Returns null if none exists.
     *
     * @param string $key The key or alias of the `Option`
     *
     * @return \Titon\Console\Option|null
     */
    public function getOption(string $key): ?Option {
        return $this->options->get($key);
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
            if ($this->parseArgument($val)) {
                continue;
            }

            $this->invalid[] = $val;
        }
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
        // The first `argument` parsed needs to be a valid command
        if (is_null($this->command)) {
            if (!$this->commands->contains($input['raw'])) {
                throw new InvalidCommandException("Command `{$input['raw']}` is not a valid command.");
            }

            $this->command = $this->commands->get($input['raw']);

            return true;
        }

        foreach ($this->arguments as $argument) {
            if (is_null($argument->getValue())) {
                $argument->setValue($input['raw']);

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

            return true;
        }

        foreach ($this->flags as $name => $flag) {
            if ($key === $flag->getNegativeAlias()) {
                invariant($flag instanceof Flag, "Must be a Flag.");

                $flag->setValue(0);

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

        return true;
    }

    /**
     * Set the input to be parsed.
     *
     * @param array<string> $args   The input to be parsed
     *
     * @return $this
     */
    public function setInput(array<string> $args): this {
        $this->input = new InputLexer($args);

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