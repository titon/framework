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

        $date = date(DateTime::RFC3339);
        $stub = new MonitorStub();

        $this->assertFileExists($this->vfs->path($path));

        $this->assertEquals('[' . $date . '] Titon\Debug\Annotation\MonitorStub was instantiated in /vagrant/tests/Titon/Debug/Annotation/MonitorTest.hh on line 35. [/]' . PHP_EOL, file_get_contents($this->vfs->path($path)));
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
