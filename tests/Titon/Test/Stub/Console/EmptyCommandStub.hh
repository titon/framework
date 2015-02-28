<?hh // strict

namespace Titon\Test\Stub\Console;

use Titon\Console\Command\AbstractCommand;
use Titon\Console\InputDefinition\Flag;

class EmptyCommandStub extends AbstractCommand {

    public function configure() {
        $this->setName('command');
    }

    public function run() {

    }
}