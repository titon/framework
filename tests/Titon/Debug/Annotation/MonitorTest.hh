<?hh
namespace Titon\Debug\Annotation;

use Titon\Annotation\WiresAnnotations;
use Titon\Debug\Debugger;
use Titon\Debug\Logger;
use Titon\Test\TestCase;
use DateTime;

class MonitorTest extends TestCase {

    protected function setUp() {
        parent::setUp();

        $this->setupVFS();
        $this->vfs->createDirectory('/logs/');

        Debugger::setLogger(new Logger($this->vfs->path('/logs/')));
    }

    public function testCallbackIsTriggered() {
        $this->assertEquals('', MonitorStub::$triggered);

        $stub = new MonitorStub();

        $this->assertEquals('Titon\Debug\Annotation\MonitorStub', MonitorStub::$triggered);
    }

    public function testMessageIsLoggedWhenClassIsInstantiated() {
        $path = '/logs/info-' . date('Y-m-d') . '.log';

        $this->assertFileNotExists($this->vfs->path($path));

        $stub = new MonitorStub();

        $this->assertFileExists($this->vfs->path($path));

        $this->assertRegExp('/^\[' . self::DATE_RFC3339_REGEX . '\] ' . preg_quote('Titon\Debug\Annotation\MonitorStub', '/') . ' was instantiated in ' . preg_quote(TEST_DIR . '/Titon/Debug/Annotation/MonitorTest.hh', '/') . '/', file_get_contents($this->vfs->path($path)));
    }

}

<<Monitor('Titon\Debug\Annotation\MonitorStub::testCallback')>>
class MonitorStub {
    use WiresAnnotations;

    public static string $triggered = '';

    public function __construct() {
        $this->wireClassAnnotation('Monitor');
    }

    public static function testCallback<T>(T $class, string $method = ''): void {
        static::$triggered = get_class($class);
    }
}
