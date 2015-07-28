<?hh
namespace Titon\Debug;

use Titon\Debug\Dumper\CliDumper;
use Titon\Debug\Exception\FatalErrorException;
use Titon\Test\TestCase;
use \ErrorException;

class DebuggerTest extends TestCase {

    protected function setUp(): void {
        parent::setUp();

        $this->vfs()->createDirectory('/logs/');

        Debugger::enable();
        Debugger::setLogger(new Logger($this->vfs()->path('/logs/')));

        set_error_handler(class_meth('Titon\Debug\Debugger', 'handleError'));
        set_exception_handler(class_meth('Titon\Debug\Debugger', 'handleException'));
    }

    protected function tearDown(): void {
        parent::tearDown();

        // Reset back to old handlers
        set_error_handler(null);
        set_exception_handler(null);
    }

    public function testExport(): void {
        $this->assertEquals(123, Debugger::export(123));
        $this->assertEquals("[\n\t0 => 123,\n]", Debugger::export([123]));
    }

    public function testGetError(): void {
        $this->assertEquals('Error', Debugger::getError(E_ERROR));
        $this->assertEquals('Core Warning', Debugger::getError(E_CORE_WARNING));
        $this->assertEquals('Unknown Error', Debugger::getError(-123));
    }

    public function testGetSetDumper(): void {
        $dumper = new CliDumper();

        Debugger::setDumper($dumper);

        $this->assertEquals($dumper, Debugger::getDumper());
    }

    public function testGetSetHandler(): void {
        $handler = ($exception) ==> {};

        Debugger::setHandler($handler);

        $this->assertEquals($handler, Debugger::getHandler());
    }

    public function testGetSetLogger(): void {
        $logger = new Logger($this->vfs()->path('/logs/'));

        Debugger::setLogger($logger);

        $this->assertEquals($logger, Debugger::getLogger());
    }

    public function testHandleError(): void {
        ob_start();
        Debugger::handleError(E_WARNING, 'Message');
        $actual = ob_get_clean();

        $this->assertRegExp('/^ErrorException/', $actual);
    }

    /**
     * @expectedException \ErrorException
     */
    public function testHandleErrorThrowsErrors(): void {
        Debugger::handleError(E_ERROR, 'Message');
    }

    public function testHandleErrorDoesntThrowsNotice(): void {
        ob_start();
        Debugger::handleError(E_NOTICE, 'Message');
        $actual = ob_get_clean();

        $this->assertRegExp('/^ErrorException - Message/', $actual);
    }

    public function testHandleErrorTriggered(): void {
        // UNSAFE
        ob_start();
        strpos();
        $actual = ob_get_clean();

        $this->assertRegExp('/^ErrorException/', $actual);
    }

    public function testHandleErrorTriggeredNoReporting(): void {
        // UNSAFE
        Debugger::disable();

        $this->assertFileNotExists($this->vfs()->path('/logs/warning-' . date('Y-m-d') . '.log'));

        ob_start();
        strpos();
        $actual = ob_get_clean();

        $this->assertEquals('', $actual);
        $this->assertFileExists($this->vfs()->path('/logs/warning-' . date('Y-m-d') . '.log'));
    }

    /**
     * @expectedException \HH\InvariantException
     */
    public function testHandleInvariant(): void {
        $this->assertFileNotExists($this->vfs()->path('/logs/info-' . date('Y-m-d') . '.log'));

        invariant_violation('Something failed!', 'foo', 'bar');

        $this->assertFileExists($this->vfs()->path('/logs/info-' . date('Y-m-d') . '.log'));
    }

    public function testLogException(): void {
        $date = date('Y-m-d');

        $this->assertFileNotExists($this->vfs()->path('/logs/error-' . $date . '.log'));
        $this->assertFileNotExists($this->vfs()->path('/logs/notice-' . $date . '.log'));

        Debugger::logException(new ErrorException('Message', E_ERROR));
        Debugger::logException(new ErrorException('Message', E_USER_NOTICE));

        $this->assertFileExists($this->vfs()->path('/logs/error-' . $date . '.log'));
        $this->assertFileExists($this->vfs()->path('/logs/notice-' . $date . '.log'));
    }

    public function testMapErrorCode(): void {
        $this->assertEquals(shape('error' => 'User Error', 'level' => 'error'), Debugger::mapErrorCode(E_USER_ERROR));
        $this->assertEquals(shape('error' => 'Core Warning', 'level' => 'warning'), Debugger::mapErrorCode(E_CORE_WARNING));
        $this->assertEquals(shape('error' => 'InvalidArgumentException', 'level' => 'debug'), Debugger::mapErrorCode(new \InvalidArgumentException()));
        $this->assertEquals(shape('error' => 'Titon\Debug\Exception\FatalErrorException', 'level' => 'error'), Debugger::mapErrorCode(new FatalErrorException('Message', E_ERROR)));
        $this->assertEquals(shape('error' => 'Strict Notice', 'level' => 'info'), Debugger::mapErrorCode(new ErrorException('Message', E_STRICT)));
    }

    public function testParseType(): void {
        $this->assertEquals('string', Debugger::parseType('foobar'));
        $this->assertEquals('integer', Debugger::parseType(123));
        $this->assertEquals('boolean', Debugger::parseType(true));
        $this->assertEquals('array', Debugger::parseType([]));
        $this->assertEquals('object', Debugger::parseType(new \stdClass()));
        $this->assertEquals('null', Debugger::parseType(null));
        $this->assertEquals('callable', Debugger::parseType(function() {}));
        $this->assertEquals('callable', Debugger::parseType(() ==> {}));
    }

    public function testParseValue(): void {
        $this->assertEquals(12345, Debugger::parseValue('12345'));
        $this->assertEquals('true', Debugger::parseValue(true));
        $this->assertEquals('null', Debugger::parseValue(null));
        $this->assertEquals('{Titon\Debug\Debugger}', Debugger::parseValue(new Debugger()));
        $this->assertEquals('"string"', Debugger::parseValue('string'));
        $this->assertEquals('"&lt;b&gt;string&lt;/b&gt;"', Debugger::parseValue('<b>string</b>'));
        $this->assertEquals('[]', Debugger::parseValue([]));
        $this->assertEquals('[123, "foo", null, true]', Debugger::parseValue([123, 'foo', null, true]));
        $this->assertEquals("[...]", Debugger::parseValue([123, 'foo', null, true], false, 3));
        $this->assertEquals('Map {"bar"}', Debugger::parseValue(Map {'foo' => 'bar'}));
        $this->assertEquals('Vector {"foo"}', Debugger::parseValue(Vector {'foo'}));
        $this->assertEquals('Set {"foo"}', Debugger::parseValue(Set {'foo'}));
        $this->assertEquals('Pair {"foo", 123}', Debugger::parseValue(Pair {'foo', 123}));

        $f = fopen('php://input', 'r');
        $this->assertEquals('stream', Debugger::parseValue($f));
        fclose($f);
    }


}
