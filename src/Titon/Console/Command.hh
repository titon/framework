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
     * @return \Titon\Console\InputBag<Argument>
     */
    public function getArguments(): InputBag<Argument>;

    /**
     * Retrieve the description of the `Command`.
     *
     * @return string
     */
    public function getDescription(): string;

    /**
     * Retrieve all of the `Flag` objects registered specifically for this command.
     *
     * @return \Titon\Console\InputBag<Flag>
     */
    public function getFlags(): InputBag<Flag>;

    /**
     * Retrieve the name of the `Command`.
     *
     * @return string
     */
    public function getName(): string;

    /**
     * Retrieve all of the `Option` objects registered specifically for this command.
     *
     * @return \Titon\Console\InputBag<Option>
     */
    public function getOptions(): InputBag<Option>;

    /**
     * The method that stores the code to be executed when the `Command` is run.
     *
     * @return void
     */
    public function run(): void;
}