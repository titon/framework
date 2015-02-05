<?hh
namespace Titon\Debug\Dumper;

use Titon\Debug\Exception\FatalErrorException;
use Titon\Test\TestCase;
use Exception;

/**
 * @property \Titon\Debug\Dumper\CliDumper $object
 */
class CliDumperTest extends TestCase {

    protected function setUp() {
        parent::setUp();

        $this->object = new CliDumper();
    }

    public function testBacktrace() {
        $line = explode(PHP_EOL, $this->object->backtrace())[0];

        $this->assertEquals('#01: Titon\Debug\Dumper\CliDumper->backtrace() - [tests]Titon/Debug/Dumper/CliDumperTest.hh:20', $line);
    }

    public function testBacktraceWithException() {
        $line = explode(PHP_EOL, $this->object->backtrace(new Exception('Systems critical!')))[0];

        $this->assertEquals('#01: Titon\Debug\Dumper\CliDumperTest->testBacktraceWithException() - [internal]:0', $line);
    }

    public function testDebug() {
        $this->assertEquals(
            '[tests]Titon/Debug/Dumper/CliDumperTest.hh:35' . PHP_EOL .
            '---------------------------------------------' . PHP_EOL .
            'foo', $this->object->debug('foo'));
    }

    public function testDebugWithMultipleVariables() {
        $this->assertEquals(
            '[tests]Titon/Debug/Dumper/CliDumperTest.hh:44' . PHP_EOL .
            '---------------------------------------------' . PHP_EOL .
            'foo' . PHP_EOL .
            '---------------------------------------------' . PHP_EOL .
            '123', $this->object->debug('foo', 123));
    }

    public function testDump() {
        $this->assertEquals(
            '[tests]Titon/Debug/Dumper/CliDumperTest.hh:51' . PHP_EOL .
            '---------------------------------------------' . PHP_EOL .
            'string(3) "foo"', $this->object->dump('foo'));
    }

    public function testDumpWithMultipleVariables() {
        $this->assertEquals(
            '[tests]Titon/Debug/Dumper/CliDumperTest.hh:60' . PHP_EOL .
            '---------------------------------------------' . PHP_EOL .
            'string(3) "foo"' . PHP_EOL .
            '---------------------------------------------' . PHP_EOL .
            'int(123)', $this->object->dump('foo', 123));
    }

    public function testInspect() {
        $line = explode(PHP_EOL, $this->object->inspect(new FatalErrorException('Systems critical!')))[0];

        $this->assertEquals('Titon\Debug\Exception\FatalErrorException - Systems critical!', $line);
    }

}
