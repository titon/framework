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


class Input {

    protected InputBag<Argument> $arguments;

    protected ?Command $command;

    protected CommandMap $commands = Map {};

    protected InputBag<Flag> $flags;

    protected InputLexer $input;

    protected Vector<RawInput> $invalid = Vector {};

    protected InputBag<Option> $options;

    /**
     * Construct a new instance of Input
     *
     * @param ?array<mixed> $args
     */
    public function __construct(?array<string> $args = null) {
        if (is_null($args)) {
            $args = array_slice(Server::get('argv'), 1);
        }

        $this->input = new InputLexer($args);
        $this->flags = new InputBag();
        $this->options = new InputBag();
        $this->arguments = new InputBag();
    }

    /**
     * Add a new `Argument` candidate to be parsed from input.
     *
     * @param Argument $argument
     *
     * @return $this
     */
    public function addArgument(Argument $argument): this {
        $this->arguments->set($argument->getName(), $argument);

        return $this;
    }

    public function addCommand(Command $command): this {
        $command->configure();
        $this->commands[$command->getName()] = $command;

        return $this;
    }

    /**
     * Add a new `Flag` candidate to be parsed from input.
     *
     * @param Flag $flag
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
     * @param Option $option
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
     * @return Argument|null
     */
    public function getArgument(string $key): ?Argument {
        return $this->arguments->get($key);
    }

    /**
     * Retrieve all `Argument` candidates.
     *
     * @return InputBag<Argument>
     */
    public function getArguments(): InputBag<Argument> {
        return $this->arguments;
    }

    public function getCommand(string $key): ?Command {
        return $this->commands->get($key);
    }

    public function getCommands(): CommandMap {
        return $this->commands;
    }

    /**
     * Retrieve a `Flag` by its key or alias. Returns null if none exists.
     *
     * @param string $key The key or alias of the `Flag`
     *
     * @return Flag|null
     */
    public function getFlag(string $key): ?Flag {
        return $this->flags->get($key);
    }

    /**
     * Retrieve all `Flag` candidates.
     *
     * @return InputBag<Flag>
     */
    public function getFlags(): InputBag<Flag> {
        return $this->flags;
    }

    public function setInput(array<string> $args): this {
        $this->input = new InputLexer($args);

        return $this;
    }

    /**
     * Retrieve an `Option` by its key or alias. Returns null if none exists.
     *
     * @param string $key The key or alias of the `Option`
     *
     * @return Option|null
     */
    public function getOption(string $key): ?Option {
        return $this->options->get($key);
    }

    /**
     * Retrieve all `Option` candidates.
     *
     * @return InputBag<Option>
     */
    public function getOptions(): InputBag<Option> {
        return $this->options;
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
     * @param RawInput $key
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
     * @param RawInput $key
     *
     * @return bool
     */
    protected function parseFlag(RawInput $input): bool {
        $key = $input['value'];
        if (is_null($flag = $this->flags->get($key))) {
            return false;
        }

        invariant($flag instanceof Flag, "Must be a Flag.");

        if ($flag->isStackable()) {
            $flag->increaseValue();
        } else {
            $flag->setValue(1);
        }

        return true;
    }

    /**
     * Determine if a RawInput matches an `Option` candidate. If so, save its
     * value.
     *
     * @param RawInput $key
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
}