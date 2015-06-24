<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Console;

use Titon\Console\Bag\ArgumentBag;
use Titon\Console\Bag\FlagBag;
use Titon\Console\Bag\OptionBag;
use Titon\Console\Input;
use Titon\Console\InputDefinition\Argument;
use Titon\Console\InputDefinition\Flag;
use Titon\Console\InputDefinition\Option;
use Titon\Console\Output;

/**
 * A `Command` is a user-defined class that can to register user input and run
 * using the input provided.
 *
 * @package Titon\Console
 */
interface Command {

    /**
     * The bootstrap method that sets up name, description, and necessary parameters
     * for the `Command` to run.
     *
     * @return void
     */
    public function configure(): void;

    /**
     * Retrieve all of the `Argument` objects registered specifically for this
     * command.
     *
     * @return \Titon\Console\Bag\ArgumentBag
     */
    public function getArguments(): ArgumentBag;

    /**
     * Retrieve the description of the `Command`.
     *
     * @return string
     */
    public function getDescription(): string;

    /**
     * Retrieve all of the `Flag` objects registered specifically for this command.
     *
     * @return \Titon\Console\Bag\FlagBag
     */
    public function getFlags(): FlagBag;

    /**
     * Retrieve the name of the `Command`.
     *
     * @return string
     */
    public function getName(): string;

    /**
     * Retrieve all of the `Option` objects registered specifically for this command.
     *
     * @return \Titon\Console\Bag\OptionBag
     */
    public function getOptions(): OptionBag;

    /**
     * This method prepares the command for execution by registering all of its
     * command-specific argumenets, flags, and options with the `Input` before
     * parsing.
     *
     * @return $this
     */
    public function registerInput(): this;

    /**
     * The method that stores the code to be executed when the `Command` is run.
     *
     * @return void
     */
    public function run(): void;

    /**
     * Set the `Input` object for the commandd.
     *
     * @param \Titon\Console\Input $input The `Input` object
     *
     * @return $this
     */
    public function setInput(Input $input): this;

    /**
     * Set the `Output` object for the commandd.
     *
     * @param \Titon\Console\Output $input The `Output` object
     *
     * @return $this
     */
    public function setOutput(Output $output): this;
}
