<?hh

use Titon\Test\TestCase;

use Titon\Console\Arguments;
use Titon\Console\InputDefinition\Flag;
use Titon\Console\InputDefinition\Option;
use Titon\Console\InputDefinition\Argument;

class ArgumentParserTest extends TestCase {

    public function testParseFlags() {
        /*
         * Check basic flag
         */
        $args = new Arguments([
            '--foo',
        ]);
        $args->addFlag((new Flag('foo'))->alias('f'));
        $args->parse();

        $this->assertEquals(1, $args->getFlag('foo')->getValue());
        $this->assertEquals(1, $args->getFlag('f')->getValue());

        $args = new Arguments([
            '-f',
        ]);
        $args->addFlag((new Flag('foo'))->alias('f'));
        $args->parse();

        $this->assertEquals(1, $args->getFlag('foo')->getValue());
        $this->assertEquals(1, $args->getFlag('f')->getValue());

        /*
         * Check stacked, but different, flags
         */
        $args = new Arguments([
            '-fb',
        ]);
        $args->addFlag((new Flag('foo'))->alias('f'));
        $args->addFlag((new Flag('bar'))->alias('b'));
        $args->parse();

        $this->assertEquals(1, $args->getFlag('foo')->getValue());
        $this->assertEquals(1, $args->getFlag('bar')->getValue());

        /*
         * Check stacked flag
         */
        $args = new Arguments([
            '-vvv',
        ]);
        $args->addFlag((new Flag('v'))->setStackable(true));
        $args->parse();

        $this->assertEquals(3, $args->getFlag('v')->getValue());
    }

    public function testParseOptions() {
        $args = new Arguments([
            '--name',
            'Alex Phillips',
        ]);
        $args->addOption((new Option('name'))->alias('n'));
        $args->addArgument(new Argument('bar', 'Bar!'));
        $args->parse();

        $this->assertEquals('Alex Phillips', $args->getOption('name')->getValue());
        $this->assertEquals('Alex Phillips', $args->getOption('n')->getValue());

        $args = new Arguments([
            '--name',
            'Alex Phillips',
        ]);
        $args->addOption((new Option('name'))->alias('n'));
        $args->addArgument(new Argument('bar', 'Bar!'));
        $args->parse();

        $this->assertEquals('Alex Phillips', $args->getOption('name')->getValue());
        $this->assertEquals('Alex Phillips', $args->getOption('n')->getValue());

        $args = new Arguments([
            '-n',
            'Alex Phillips',
        ]);
        $args->addOption((new Option('name'))->alias('n'));
        $args->addArgument(new Argument('bar', 'Bar!'));
        $args->parse();

        $this->assertEquals('Alex Phillips', $args->getOption('name')->getValue());
        $this->assertEquals('Alex Phillips', $args->getOption('n')->getValue());

        $args = new Arguments([
            '--name="Alex Phillips"',
        ]);
        $args->addOption((new Option('name'))->alias('n'));
        $args->addArgument(new Argument('bar', 'Bar!'));
        $args->parse();

        $this->assertEquals('Alex Phillips', $args->getOption('name')->getValue());
        $this->assertEquals('Alex Phillips', $args->getOption('n')->getValue());
    }

    public function testParseArguments() {
        $args = new Arguments([
            'Alex Phillips',
        ]);
        $args->addArgument(new Argument('name'));
        $args->parse();

        $this->assertEquals('Alex Phillips', $args->getArgument('name')->getValue());
    }

    public function testMixedArguments() {
        $args = new Arguments([
            'Alex Phillips',
            '-fb',
            '--baz="woot"',
        ]);
        $args->addFlag((new Flag('bar'))->alias('b'));
        $args->addOption(new Option('baz'));
        $args->addFlag((new Flag('foo'))->alias('f'));
        $args->addArgument(new Argument('name'));
        $args->parse();

        $this->assertEquals('Alex Phillips', $args->getArgument('name')->getValue());
        $this->assertEquals(1, $args->getFlag('foo')->getValue());
        $this->assertEquals(1, $args->getFlag('f')->getValue());
        $this->assertEquals(1, $args->getFlag('bar')->getValue());
        $this->assertEquals(1, $args->getFlag('b')->getValue());
        $this->assertEquals('woot', $args->getOption('baz')->getValue());
    }
}