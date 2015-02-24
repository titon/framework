<?hh

use Titon\Test\TestCase;

use Titon\Console\Arguments;
use Titon\Console\InputDefinition\Flag;
use Titon\Console\InputDefinition\Option;
use Titon\Console\InputDefinition\Argument;

class ArgumentParserTest extends TestCase {

    public function testParseFlags() {
        $args = new Arguments([
            '-f',
            'FooBar',
            '--name',
            'Alex Phillips',
        ]);

        $args->addFlag(new Flag('f', 'Foo!'));
        $args->addOption(new Option('name', 'Your name'));
        $args->addArgument(new Argument('bar', 'Bar!'));

        $args->parse();

        $this->assertEquals(1, $args->getFlag('f')->getValue());
        $this->assertEquals('Alex Phillips', $args->getOption('name')->getValue());
        $this->assertEquals('FooBar', $args->getArgument('bar')->getValue());
    }
}