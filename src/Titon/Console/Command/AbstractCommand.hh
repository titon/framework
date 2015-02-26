<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Console\Command;

use Titon\Console\ArgumentBag;
use Titon\Console\Command;
use Titon\Console\Arguments;
use Titon\Console\InputDefinition\Argument;
use Titon\Console\InputDefinition\Flag;
use Titon\Console\InputDefinition\Option;

abstract class AbstractCommand implements Command {

    protected ArgumentBag $arguments;

    protected ?Arguments $input;

    protected ?string $description;

    protected ArgumentBag $flags;

    protected string $name;

    protected ArgumentBag $options;

    public function __construct() {
        $this->arguments = new ArgumentBag();
        $this->flags = new ArgumentBag();
        $this->options = new ArgumentBag();
    }

    public function setInput(Arguments $input): this {
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

    protected function getArgument(string $key): ?string {
        if ($argument = $this->input->getArgument($key)) {
            return $argument->getValue();
        }

        return null;
    }

    protected function getArguments(): ArgumentBag {
        return $this->arguments;
    }

    protected function getFlag(string $key): ?int {
        if ($flag = $this->input->getFlag($key)) {
            return $flag->getValue();
        }

        return null;
    }

    protected function getFlags(): ArgumentBag {
        return $this->flags;
    }

    protected function getOption(string $key): ?string {
        if ($option = $this->input->getOption($key)) {
            return $option->getValue();
        }

        return null;
    }

    protected function getOptions(): ArgumentBag {
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
    }
}