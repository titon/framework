<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Console;

class Console {

    protected CommandMap $commands = Map {};

    public function __construct() {

    }

    public function addCommand(Command $command): this {
        $this->commands[$command->getName()] = $command;

        return $this;
    }
}