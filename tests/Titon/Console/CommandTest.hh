<?hh

use Titon\Test\TestCase;

use Titon\Console\Arguments;
use Titon\Console\InputDefinition\Flag;
use Titon\Console\InputDefinition\Option;
use Titon\Console\InputDefinition\Argument;

class CommandTest extends TestCase {

    public function testAddArguments() {
        $args = new Arguments([
            '-v'
        ]);
        $args->addFlag(new Flag('help', 'Show this help screen'));

        $command = new TestCommand();
        $command->setInput($args);
        $command->configure();
        $command->registerInput();

        $args->parse();

        $this->assertEquals(2, count($args->getFlags()));
        $this->assertEquals(1, $args->getFlag('v')->getValue());
    }
}

class TestCommand extends \Titon\Console\Command\AbstractCommand {

    public function configure() {
        $this->addFlag((new Flag('verbose', 'Set command verbosity'))->alias('v'));
    }

    public function run() {

    }
}