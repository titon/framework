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

abstract class AbstractCommand implements Command {

    protected InputBag<Argument> $arguments;

    protected Input $input;

    protected string $description;

    protected InputBag<Flag> $flags;

    protected string $name;

    protected InputBag<Option> $options;

    public function __construct() {
        $this->input = new Input();
        $this->arguments = new InputBag();
        $this->flags = new InputBag();
        $this->options = new InputBag();
    }

    public function setInput(Input $input): this {
        $this->input = $input;

        return $this;
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

    protected function getArgument(string $key): mixed {
        if ($argument = $this->input->getArgument($key)) {
            return $argument->getValue();
        }

        return null;
    }

    public function getArguments(): InputBag<Argument> {
        return $this->arguments;
    }

    public function getDescription(): string {
        return $this->description;
    }

    protected function getFlag(string $key): ?int {
        if ($flag = $this->input->getFlag($key)) {
            return $flag->getValue();
        }

        return null;
    }

    public function getFlags(): InputBag<Flag> {
        return $this->flags;
    }

    public function getName(): string {
        return $this->name;
    }

    protected function getOption(string $key): mixed {
        if ($option = $this->input->getOption($key)) {
            return $option->getValue();
        }

        return null;
    }

    public function getOptions(): InputBag<Option> {
        return $this->options;
    }

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
}