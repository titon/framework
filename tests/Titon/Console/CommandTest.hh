<?hh

namespace Titon\Console;

use Titon\Test\TestCase;
use Titon\Test\Stub\Console\CommandStub;
use Titon\Console\InputDefinition\Flag;
use Titon\Console\InputDefinition\Option;
use Titon\Console\InputDefinition\Argument;

class CommandTest extends TestCase {

    public function testAddArguments(): void {
        $args = new Input(Vector {
            '--table'
        });

        $command = new CommandStub($args, new Output());
        $command->setInput($args);
        $command->configure();
        $command->registerInput();

        $args->parse();

        $this->assertEquals(3, count($args->getFlags()));
        $this->assertEquals(2, count($args->getOptions()));
        $this->assertEquals(1, count($args->getArguments()));
        $this->assertEquals(1, $args->getFlag('table')->getValue());
    }
}