<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Console;

use Titon\Common\ArgumentList;
use Titon\Console\InputDefinition\Argument;
use Titon\Console\InputDefinition\Flag;
use Titon\Console\InputDefinition\Option;
use Titon\Console\Exception\MissingValueException;

class Input {

    protected InputBag<Argument> $arguments;

    protected CommandMap $commands = Map {};

    protected InputBag<Flag> $flags;

    protected InputLexer $input;

    protected Vector<RawInput> $invalid = Vector {};

    protected InputBag<Option> $options;

    public function __construct(?array<string> $args = null) {
        if (is_null($args)) {
            $args = array_slice($_SERVER['argv'], 1);
        }

        $this->input = new InputLexer($args);
        $this->flags = new InputBag();
        $this->options = new InputBag();
        $this->arguments = new InputBag();
    }

    public function addArgument(Argument $argument): this {
        $this->arguments->set($argument->getName(), $argument);

        return $this;
    }

    public function addFlag(Flag $flag): this {
        $this->flags->set($flag->getName(), $flag);

        return $this;
    }

    public function addOption(Option $option): this {
        $this->options->set($option->getName(), $option);

        return $this;
    }

    public function getArgument(string $key): ?Argument {
        if (!is_null($argument = $this->arguments->get($key))) {
            return $argument;
        }

        return null;
    }

    public function getArguments(): InputBag<Argument> {
        return $this->arguments;
    }

    public function getFlag(string $key): ?Flag {
        if (!is_null($flag = $this->flags->get($key))) {
            return $flag;
        }

        foreach ($this->flags as $flag) {
            if ($key === $flag->getName() || $key === $flag->getAlias()) {
                return $flag;
            }
        }

        return null;
    }

    public function getFlags(): InputBag<Flag> {
        return $this->flags;
    }

    public function getOption(string $key): ?Option {
        if (!is_null($option = $this->options->get($key))) {
            return $option;
        }

        return null;
    }

    public function getOptions(): InputBag<Option> {
        return $this->options;
    }

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

    protected function parseArgument(RawInput $key): bool {
        foreach ($this->arguments as $argument) {
            if (is_null($argument->getValue())) {
                $argument->setValue($key['raw']);

                return true;
            }
        }

        return false;
    }

    protected function parseFlag(RawInput $key): bool {
        $key = $key['value'];
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

    protected function parseOption(RawInput $key): bool {
        $key = $key['value'];
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