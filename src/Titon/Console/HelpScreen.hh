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

/**
 * The `HelpScreen` class renders out a usage screen given the available `Flag`,
 * `Option`, and `Argument` objects avaiable as well as available commands that
 * can be executed.
 *
 * @package Titon\Console
 */
class HelpScreen {

    /**
     * The available `Argument` objects accepted.
     *
     * @var \Titon\Console\InputBag<Argument>
     */
    protected InputBag<Argument> $arguments;

    /**
     * The current `Command` the `HelpScreen` refers to.
     *
     * @var \Titon\Console\Command|null
     */
    protected ?Command $command;

    /**
     * The available `Command` objects available.
     *
     * @var CommandMap
     */
    protected CommandMap $commands;

    /**
     * The available `Flag` objects accepted.
     *
     * @var \Titon\Console\InputBag<Flag>
     */
    protected InputBag<Flag> $flags;

    /**
     * The optional `name` of the application when not outputting a `HelpScreen`
     * for a specific `Command`.
     *
     * @var string|null
     */
    protected ?string $name;

    /**
     * The available `Option` objects accepted.
     *
     * @var \Titon\Console\InputBag<Option>
     */
    protected InputBag<Option> $options;

    /**
     * Construct a new instance of the `HelpScreen`.
     *
     * @param \Titon\Console\Input $input   The `Input` to read all available
     *                                      parameters from
     */
    public function __construct(Input $input) {
        $this->commands = $input->getCommands();
        $this->arguments = $input->getArguments();
        $this->flags = $input->getFlags();
        $this->options = $input->getOptions();
    }

    /**
     * Build and return the markup for the `HelpScreen`.
     *
     * @return string
     */
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

    /**
     * Build the list of available `Command` objects that can be called and their
     * descriptions.
     *
     * @return string
     */
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

    /**
     * Build and return the markup for the heading of the `HelpScreen`. This is
     * either the name of the application (when not rendering for a specific
     * `Command`) or the name and description of the `Command`.
     *
     * @return string
     */
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

    /**
     * Build and return a specific section of available `Input` objects the user
     * may specify.
     *
     * @param \Titon\Console\InputBag<T> $arguments The parameters to build information for
     *
     * @return string
     */
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

    /**
     * When rendering a for a `Command`, this method builds and returns the usage.
     *
     * @return string
     */
    protected function renderUsage(): string {
        $usage = Vector {};
        if (!is_null($this->command)) {
            $command = $this->command;

            // Setting local variable and `invariant`ing it to quiet type checker
            invariant(!is_null($command), "Must be a Command.");

            $usage[] = $command->getName();

            foreach ($command->getArguments() as $argument) {
                $arg = $argument->getName();
                if ($argument->getMode() === AbstractInputDefinition::MODE_OPTIONAL) {
                    $usage[] = "[$arg]";
                }
            }
            foreach ($command->getArguments() as $argument) {
                $arg = $argument->getName();
                if ($argument->getAlias()) {
                    $arg = "$arg|{$argument->getAlias()}";
                }
                if ($argument->getMode() === AbstractInputDefinition::MODE_OPTIONAL) {
                    $usage[] = "[$arg]";
                }
            }
            foreach ($command->getArguments() as $argument) {
                $arg = $argument->getName();
                if ($argument->getAlias()) {
                    $arg = "$arg|{$argument->getAlias()}";
                }

                $arg = "$arg=\"...\"";
                if ($argument->getMode() === AbstractInputDefinition::MODE_OPTIONAL) {
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

    /**
     * Set the `Argument` objects to render information for.
     *
     * @param \Titon\Console\InputBag<Argument> $arguments The `Argument` objects avaiable
     *
     * @return $this
     */
    public function setArguments(InputBag<Argument> $arguments): this {
        $this->arguments = $arguments;

        return $this;
    }

    /**
     * Set the `Command` to render a the help screen for.
     *
     * @param \Titon\Console\InputBag<Argument> $arguments The `Command` object
     *
     * @return $this
     */
    public function setCommand(Command $command): this {
        $this->command = $command;

        return $this;
    }

    /**
     * Set the `Command` objects to render information for.
     *
     * @param \Titon\Console\CommandMap $arguments The `Command` objects avaiable
     *
     * @return $this
     */
    public function setCommands(CommandMap $commands): this {
        $this->commands = $commands;

        return $this;
    }

    /**
     * Set the `Flag` objects to render information for.
     *
     * @param \Titon\Console\InputBag<Flag> $arguments The `Flag` objects avaiable
     *
     * @return $this
     */
    public function setFlags(InputBag<Flag> $flags): this {
        $this->flags = $flags;

        return $this;
    }

    /**
     * Set the `Input` the help screen should read all avaiable parameters and
     * commands from.
     *
     * @param \Titon\Console\Input $input   The `Input` object with all available
     *                                      parameters and commands
     *
     * @return $this
     */
    public function setInput(Input $input): this {
        $this->commands = $input->getCommands();
        $this->arguments = $input->getArguments();
        $this->flags = $input->getFlags();
        $this->options = $input->getOptions();

        return $this;
    }

    /**
     * Set the name of the application
     *
     * @param string $name  The name (and other information) of the console application
     *
     * @return $this
     */
    public function setName(string $name): this {
        $this->name = $name;

        return $this;
    }

    /**
     * Set the `Option` objects to render information for.
     *
     * @param \Titon\Console\InputBag<Option> $arguments The `Option` objects avaiable
     *
     * @return $this
     */
    public function setOptions(InputBag<Option> $options): this {
        $this->options = $options;

        return $this;
    }
}