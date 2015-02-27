<?hh
namespace Titon\Debug\Annotation;

use Titon\Debug\Debugger;
use Titon\Debug\Logger;
use Titon\Test\Stub\Debug\MonitorClassStub;
use Titon\Test\TestCase;

class MonitorTest extends TestCase {

    protected function setUp(): void {
        parent::setUp();

        $this->vfs()->createDirectory('/logs/');

        Debugger::setLogger(new Logger($this->vfs()->path('/logs/')));
    }

    public function testCallbackIsTriggered(): void {
        $this->assertEquals('', MonitorClassStub::$triggered);

        $stub = new MonitorClassStub();

        $this->assertEquals('Titon\Test\Stub\Debug\MonitorClassStub', MonitorClassStub::$triggered);
    }

    public function testMessageIsLoggedWhenClassIsInstantiated(): void {
        $path = '/logs/info-' . date('Y-m-d') . '.log';

        $this->assertFileNotExists($this->vfs()->path($path));

        $stub = new MonitorClassStub();

        $this->assertFileExists($this->vfs()->path($path));

        $this->assertRegExp('/^\[' . self::DATE_RFC3339_REGEX . '\] ' . preg_quote('Titon\Test\Stub\Debug\MonitorClassStub', '/') . ' was instantiated in ' . preg_quote(TEST_DIR . '/Titon/Debug/Annotation/MonitorTest.hh', '/') . '/', file_get_contents($this->vfs()->path($path)));
    }

}
