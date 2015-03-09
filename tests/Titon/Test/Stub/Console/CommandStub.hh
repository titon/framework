<?hh // strict

namespace Titon\Test\Stub\Console;

use Titon\Console\Command\AbstractCommand;
use Titon\Console\InputDefinition\Flag;

class CommandStub extends AbstractCommand {

    public function configure() {
        $this->setName('command');
        $this->addFlag((new Flag('verbose', 'Set command verbosity'))
            ->alias('v'));
    }

    public function run() {

    }
}