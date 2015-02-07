<?hh
namespace Titon\Debug;

use Titon\Debug\Exception\FatalErrorException;
use Titon\Test\TestCase;

class GlobalFunctionTest extends TestCase {

    public function testBacktrace() {
        ob_start();
        \backtrace();
        $actual = ob_get_clean();

        $this->assertRegExp('/^#01/', $actual);
    }

    public function testBacktraceNoReporting() {
        Debugger::disable();

        ob_start();
        \backtrace();
        $actual = ob_get_clean();

        $this->assertEquals('', $actual);

        Debugger::enable();
    }

    public function testDebug() {
        ob_start();
        \debug(1);
        $actual = ob_get_clean();

        $this->assertRegExp('/^\[src\]/', $actual);
    }

    public function testDebugNoReporting() {
        Debugger::disable();

        ob_start();
        \debug(1);
        $actual = ob_get_clean();

        $this->assertEquals('', $actual);

        Debugger::enable();
    }

    public function testDump() {
        ob_start();
        \dump(1);
        $actual = ob_get_clean();

        $this->assertRegExp('/^\[src\]/', $actual);
    }

    public function testDumpNoReporting() {
        Debugger::disable();

        ob_start();
        \dump(1);
        $actual = ob_get_clean();

        $this->assertEquals('', $actual);

        Debugger::enable();
    }

    public function testInspect() {
        ob_start();
        \inspect(new FatalErrorException('Systems critical!'));
        $actual = ob_get_clean();

        $this->assertRegExp('/^Titon\\\\Debug\\\\Exception\\\\FatalErrorException/', $actual);
    }

    public function testInspectNoReporting() {
        Debugger::disable();

        ob_start();
        \inspect(new FatalErrorException('Systems critical!'));
        $actual = ob_get_clean();

        $this->assertEquals('', $actual);

        Debugger::enable();
    }

    public function testExport() {
        ob_start();
        \export('foo');
        $actual = ob_get_clean();

        $this->assertEquals("'foo'", $actual);
    }

    public function testExportNoReporting() {
        Debugger::disable();

        ob_start();
        \export('foo');
        $actual = ob_get_clean();

        $this->assertEquals('', $actual);

        Debugger::enable();
    }

}
