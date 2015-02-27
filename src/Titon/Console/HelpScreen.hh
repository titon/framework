<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Console;

use Titon\Console\InputDefinition\Argument;
use Titon\Console\InputDefinition\Flag;
use Titon\Console\InputDefinition\Option;
use Titon\Console\InputDefinition\AbstractInputDefinition;

class HelpScreen {

    protected InputBag<Argument> $arguments;

    protected ?Command $command;

    protected CommandMap $commands;

    protected InputBag<Flag> $flags;

    protected ?string $name;

    protected InputBag<Option> $options;

    public function __construct(Input $input) {
        $this->commands = $input->getCommands();
        $this->arguments = $input->getArguments();
        $this->flags = $input->getFlags();
        $this->options = $input->getOptions();
    }

    public function render(): string {
        $retval = Vector {};

        $retval[] = $this->renderHeading();

        $retval[] = $this->renderUsage();

        if (!$this->arguments->all()->isEmpty()) {
            if ($output = $this->renderSection($this->arguments)) {
                $retval[] = "Arguments\n$output";
            }
        }
        if (!$this->flags->all()->isEmpty()) {
            if ($output = $this->renderSection($this->flags)) {
                $retval[] = "Flags\n$output";
            }
        }
        if (!$this->options->all()->isEmpty()) {
            if ($output = $this->renderSection($this->options)) {
                $retval[] = "Options\n$output";
            }
        }
        if (is_null($this->command)) {
            if (!$this->commands->isEmpty()) {
                $retval[] = $this->renderCommands();
            }
        }

        return join($retval, "\n\n");
    }

    protected function renderCommands(): string {
        ksort($this->commands);

        $maxLength = max(array_map(function(string $key): int {
            $indentation = substr_count($key, ':');
            $key = str_repeat('  ', $indentation) . $key;

            return strlen($key);
        }, $this->commands->keys()));
        $descriptionLength = 80 - 4 - $maxLength;

        $output = Vector {};
        $nestedNames = Vector {};
        foreach ($this->commands as $name => $command) {
            $nested = explode(':', $name);
            array_pop($nested);

            if (count($nested) > 0) {
                $nest = '';
                foreach ($nested as $piece) {
                    $nest = $nest ? ":$piece" : $piece;

                    if ($nestedNames->linearSearch($nest) < 0) {
                        // If we get here, then we need to list the name, but it isn't
                        // actually a command, so subtract substr_count by 1.
                        $nestedNames[] = $nest;

                        $indentation = substr_count($name, ':') - 1;
                        $output[] = str_repeat('  ', $indentation) . str_pad($nest, $maxLength);
                    }
                }
            } else {
                $nestedNames[] = $name;
            }

            $indentation = substr_count($name, ':');
            $formatted = str_repeat('  ', $indentation) . str_pad($name, $maxLength - (2 * $indentation));

            $description = explode('{{BREAK}}', wordwrap($command->getDescription(), $descriptionLength, "{{BREAK}}"));
            $formatted .= '  ' . array_shift($description);

            $pad = str_repeat(' ', $maxLength + 4);
            while ($desc = array_shift($description)) {
                $formatted .= "\n$pad$desc";
            }

            $formatted = "$formatted";

            array_push($output, $formatted);
        }

        return join($output, "\n");
    }

    protected function renderHeading(): string {
        $retval = '';

        if (!is_null($this->command)) {
            $command = $this->command;

            invariant(!is_null($command), "Must be a command.");

            if ($description = $command->getDescription()) {
                $retval = $command->getName() . ' - ' . $description;
            } else {
                $retval = $command->getName();
            }
        } else if (!is_null($this->name)) {
            $retval = $this->name;
        }

        return $retval;
    }

    protected function renderSection<T as InputDefinition>(InputBag<T> $arguments): string {
        $entries = Map {};
        foreach ($arguments as $argument) {
            $name = $argument->getFormattedName($argument->getName());
            if ($argument->getAlias()) {
                $name .= " ({$argument->getFormattedName($argument->getAlias())})";
            }
            $entries[$name] = $argument->getDescription();
        }

        $maxLength = max(array_map(function(string $key): int {
            return strlen($key);
        }, $entries->keys()));
        $descriptionLength = 80 - 4 - $maxLength;

        $output = Vector {};
        foreach ($entries as $name => $description) {
            $formatted = '  ' . str_pad($name, $maxLength);
            $description = explode('{{BREAK}}', wordwrap($description, $descriptionLength, "{{BREAK}}"));
            $formatted .= '  ' . array_shift($description);

            $pad = str_repeat(' ', $maxLength + 4);
            while ($desc = array_shift($description)) {
                $formatted .= "\n$pad$desc";
            }

            $formatted = "$formatted";

            array_push($output, $formatted);
        }

        return join($output, "\n");
    }

    protected function renderUsage(): string {
        $usage = Vector {};
        if (!is_null($this->command)) {
            $command = $this->command;

            // Setting local variable and `invariant`ing it to quiet type checker
            invariant(!is_null($command), "Must be a Command.");

            $usage[] = $command->getName();

            foreach ($command->getArguments() as $argument) {
                $arg = $argument->getName();
                if ($argument->getMode() === AbstractInputDefinition::VALUE_OPTIONAL) {
                    $usage[] = "[$arg]";
                }
            }
            foreach ($command->getArguments() as $argument) {
                $arg = $argument->getName();
                if ($argument->getAlias()) {
                    $arg = "$arg|{$argument->getAlias()}";
                }
                if ($argument->getMode() === AbstractInputDefinition::VALUE_OPTIONAL) {
                    $usage[] = "[$arg]";
                }
            }
            foreach ($command->getArguments() as $argument) {
                $arg = $argument->getName();
                if ($argument->getAlias()) {
                    $arg = "$arg|{$argument->getAlias()}";
                }

                $arg = "$arg=\"...\"";
                if ($argument->getMode() === AbstractInputDefinition::VALUE_OPTIONAL) {
                    $usage[] = "[$arg]";
                }
            }
        } else {
            $usage[] = "command";
            if (!$this->flags->all()->isEmpty()) {
                $usage[] = "[flags]";
            }
            if (!$this->options->all()->isEmpty()) {
                $usage[] = "[options]";
            }
        }

        return "Usage\n  " . join(" ", $usage);
    }

    public function setArguments(InputBag<Argument> $arguments): this {
        $this->arguments = $arguments;

        return $this;
    }

    public function setCommand(Command $command): this {
        $this->command = $command;

        return $this;
    }

    public function setCommands(CommandMap $commands): this {
        $this->commands = $commands;

        return $this;
    }

    public function setFlags(InputBag<Flag> $flags): this {
        $this->flags = $flags;

        return $this;
    }

    public function setInput(Input $input): this {
        $this->commands = $input->getCommands();
        $this->arguments = $input->getArguments();
        $this->flags = $input->getFlags();
        $this->options = $input->getOptions();

        return $this;
    }

    public function setName(string $name): this {
        $this->name = $name;

        return $this;
    }

    public function setOptions(InputBag<Option> $options): this {
        $this->options = $options;

        return $this;
    }
}