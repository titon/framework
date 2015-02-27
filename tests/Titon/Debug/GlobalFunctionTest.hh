<?hh
namespace Titon\Debug;

use Titon\Debug\Exception\FatalErrorException;
use Titon\Test\TestCase;

class GlobalFunctionTest extends TestCase {

    public function testBacktrace(): void {
        ob_start();
        \backtrace();
        $actual = ob_get_clean();

        $this->assertRegExp('/^#01/', $actual);
    }

    public function testBacktraceNoReporting(): void {
        Debugger::disable();

        ob_start();
        \backtrace();
        $actual = ob_get_clean();

        $this->assertEquals('', $actual);

        Debugger::enable();
    }

    public function testDebug(): void {
        ob_start();
        \debug(1);
        $actual = ob_get_clean();

        $this->assertRegExp('/^\[src\]/', $actual);
    }

    public function testDebugNoReporting(): void {
        Debugger::disable();

        ob_start();
        \debug(1);
        $actual = ob_get_clean();

        $this->assertEquals('', $actual);

        Debugger::enable();
    }

    public function testDump(): void {
        ob_start();
        \dump(1);
        $actual = ob_get_clean();

        $this->assertRegExp('/^\[src\]/', $actual);
    }

    public function testDumpNoReporting(): void {
        Debugger::disable();

        ob_start();
        \dump(1);
        $actual = ob_get_clean();

        $this->assertEquals('', $actual);

        Debugger::enable();
    }

    public function testInspect(): void {
        ob_start();
        \inspect(new FatalErrorException('Systems critical!'));
        $actual = ob_get_clean();

        $this->assertRegExp('/^Titon\\\\Debug\\\\Exception\\\\FatalErrorException/', $actual);
    }

    public function testInspectNoReporting(): void {
        Debugger::disable();

        ob_start();
        \inspect(new FatalErrorException('Systems critical!'));
        $actual = ob_get_clean();

        $this->assertEquals('', $actual);

        Debugger::enable();
    }

    public function testExport(): void {
        ob_start();
        \export('foo');
        $actual = ob_get_clean();

        $this->assertEquals("'foo'", $actual);
    }

    public function testExportNoReporting(): void {
        Debugger::disable();

        ob_start();
        \export('foo');
        $actual = ob_get_clean();

        $this->assertEquals('', $actual);

        Debugger::enable();
    }

}
