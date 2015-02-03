<?hh
namespace Titon\Debug;

use Titon\Debug\Exception\FatalErrorException;
use Titon\Test\TestCase;
use \ErrorException;

/**
 * @property callable $errorHandler
 * @property callable $exceptionHandler
 */
class DebuggerTest extends TestCase {

    protected function setUp() {
        parent::setUp();

        $this->setupVFS();
        $this->vfs->createDirectory('/logs/');

        Debugger::enable();
        Debugger::setLogger(new Logger($this->vfs->path('/logs/')));

        set_error_handler(class_meth('Titon\Debug\Debugger', 'handleError'));
        set_exception_handler(class_meth('Titon\Debug\Debugger', 'handleException'));
    }

    protected function tearDown() {
        parent::tearDown();

        // Reset back to old handlers
        set_error_handler(null);
        set_exception_handler(null);
    }

    public function testBacktrace() {
        $this->assertRegExp('/^<div class="titon-debug titon-backtrace">/', Debugger::backtrace());

        ob_start();
        \backtrace();
        $actual = ob_get_clean();

        $this->assertRegExp('/^<div class="titon-debug titon-backtrace">/', $actual);

        Debugger::disable();
        $this->assertEquals('', Debugger::backtrace());
    }

    public function testDebug() {
        $this->assertRegExp('/^<div class="titon-debug">/', Debugger::debug(1));

        ob_start();
        \debug(1);
        $actual = ob_get_clean();

        $this->assertRegExp('/^<div class="titon-debug">/', $actual);

        Debugger::disable();
        $this->assertEquals('', Debugger::debug(1));
    }

    public function testDump() {
        $this->assertRegExp('/^<div class="titon-debug">/', Debugger::dump(1));

        ob_start();
        \dump(1);
        $actual = ob_get_clean();

        $this->assertRegExp('/^<div class="titon-debug">/', $actual);

        Debugger::disable();
        $this->assertEquals('', Debugger::dump(1));
    }

    public function testExport() {
        $this->assertEquals(123, Debugger::export(123));
        $this->assertEquals("[\n\t0 => 123,\n]", Debugger::export([123]));
        $this->assertEquals("array(\n\t0 => 123,\n)", Debugger::export([123], false));

        ob_start();
        \export([123]);
        $actual = ob_get_clean();

        $this->assertEquals("[\n\t0 => 123,\n]", $actual);

        Debugger::disable();
        $this->assertEquals('', Debugger::export(123));
    }

    public function testGetError() {
        $this->assertEquals('Error', Debugger::getError(E_ERROR));
        $this->assertEquals('Core Warning', Debugger::getError(E_CORE_WARNING));
        $this->assertEquals('Unknown Error', Debugger::getError(-123));
    }

    public function testHandleError() {
        ob_start();
        Debugger::handleError(E_WARNING, 'Message');
        $actual = ob_get_clean();

        $this->assertRegExp('/<div class="titon-debug titon-error">/', $actual);
    }

    /**
     * @expectedException \ErrorException
     */
    public function testHandleErrorThrowsErrors() {
        Debugger::handleError(E_ERROR, 'Message');
    }

    public function testHandleErrorDoesntThrowsNotice() {
        ob_start();
        Debugger::handleError(E_NOTICE, 'Message');
        ob_get_clean();
    }

    public function testHandleErrorTriggered() {
        ob_start();
        strpos();
        $actual = ob_get_clean();

        $this->assertRegExp('/<div class="titon-debug titon-error">/', $actual);

        // With no error reporting
        Debugger::disable();

        $this->assertFileNotExists($this->vfs->path('/logs/warning-' . date('Y-m-d') . '.log'));

        ob_start();
        strpos();
        $actual = ob_get_clean();

        $this->assertEquals('', $actual);
        $this->assertFileExists($this->vfs->path('/logs/warning-' . date('Y-m-d') . '.log'));
    }

    /**
     * @expectedException \HH\InvariantException
     */
    public function testHandleInvariant() {
        $this->assertFileNotExists($this->vfs->path('/logs/info-' . date('Y-m-d') . '.log'));

        invariant_violation('Something failed!', 'foo', 'bar');

        $this->assertFileExists($this->vfs->path('/logs/info-' . date('Y-m-d') . '.log'));
    }

    public function testInspect() {
        $e = new \Exception('Foobar');

        $this->assertRegExp('/^<div class="titon-debug titon-error">/', Debugger::inspect($e));

        ob_start();
        \inspect($e);
        $actual = ob_get_clean();

        $this->assertRegExp('/^<div class="titon-debug titon-error">/', $actual);

        Debugger::disable();
        $this->assertEquals('', Debugger::inspect($e));
    }

    public function testLogException() {
        $date = date('Y-m-d');

        $this->assertFileNotExists($this->vfs->path('/logs/error-' . $date . '.log'));
        $this->assertFileNotExists($this->vfs->path('/logs/notice-' . $date . '.log'));

        Debugger::logException(new ErrorException('Message', E_ERROR));
        Debugger::logException(new ErrorException('Message', E_USER_NOTICE));

        $this->assertFileExists($this->vfs->path('/logs/error-' . $date . '.log'));
        $this->assertFileExists($this->vfs->path('/logs/notice-' . $date . '.log'));
    }

    public function testMapErrorCode() {
        $this->assertEquals(shape('error' => 'User Error', 'level' => 'error'), Debugger::mapErrorCode(E_USER_ERROR));
        $this->assertEquals(shape('error' => 'Core Warning', 'level' => 'warning'), Debugger::mapErrorCode(E_CORE_WARNING));
        $this->assertEquals(shape('error' => 'InvalidArgumentException', 'level' => 'debug'), Debugger::mapErrorCode(new \InvalidArgumentException()));
        $this->assertEquals(shape('error' => 'Titon\Debug\Exception\FatalErrorException', 'level' => 'error'), Debugger::mapErrorCode(new FatalErrorException('Message', E_ERROR)));
        $this->assertEquals(shape('error' => 'Strict Notice', 'level' => 'info'), Debugger::mapErrorCode(new ErrorException('Message', E_STRICT)));
    }

    public function testParseType() {
        $this->assertEquals('string', Debugger::parseType('foobar'));
        $this->assertEquals('integer', Debugger::parseType(123));
        $this->assertEquals('boolean', Debugger::parseType(true));
        $this->assertEquals('array', Debugger::parseType([]));
        $this->assertEquals('object', Debugger::parseType(new \stdClass()));
        $this->assertEquals('null', Debugger::parseType(null));
        $this->assertEquals('callable', Debugger::parseType(function() {}));
    }

    public function testParseValue() {
        $this->assertEquals(12345, Debugger::parseValue('12345'));
        $this->assertEquals('true', Debugger::parseValue(true));
        $this->assertEquals('null', Debugger::parseValue(null));
        $this->assertEquals('{Titon\Debug\Debugger}', Debugger::parseValue(new Debugger()));
        $this->assertEquals('"string"', Debugger::parseValue('string'));
        $this->assertEquals('"&lt;b&gt;string&lt;/b&gt;"', Debugger::parseValue('<b>string</b>'));
        $this->assertEquals('[]', Debugger::parseValue([]));
        $this->assertEquals('[123, "foo", null, true]', Debugger::parseValue([123, 'foo', null, true]));
        $this->assertEquals("[...]", Debugger::parseValue([123, 'foo', null, true], false, 3));

        $f = fopen('php://input', 'r');
        $this->assertEquals('stream', Debugger::parseValue($f));
        fclose($f);
    }

    public function testSetHandler() {
        $handler = function() {};

        Debugger::setHandler($handler);

        $this->assertEquals($handler, Debugger::getHandler());
    }

}
