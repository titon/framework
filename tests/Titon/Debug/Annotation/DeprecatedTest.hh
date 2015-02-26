<?hh // strict
namespace Titon\Debug\Annotation;

use Titon\Debug\Debugger;
use Titon\Debug\Logger;
use Titon\Test\Stub\Debug\DeprecatedClassStub;
use Titon\Test\TestCase;

class DeprecatedTest extends TestCase {

    protected function setUp(): void {
        parent::setUp();

        $this->setupVFS();
        $this->vfs->createDirectory('/logs/');

        Debugger::setLogger(new Logger($this->vfs->path('/logs/')));
    }

    public function testMessageIsLoggedWhenClassIsInstantiated(): void {
        $path = '/logs/notice-' . date('Y-m-d') . '.log';

        $this->assertFileNotExists($this->vfs->path($path));

        $stub = new DeprecatedClassStub();

        $this->assertFileExists($this->vfs->path($path));

        $this->assertRegExp('/^\[' . self::DATE_RFC3339_REGEX . '\] ' . preg_quote('Titon\Test\Stub\Debug\DeprecatedClassStub', '/') . ' is deprecated\. This is the error message\./', file_get_contents($this->vfs->path($path)));
    }

}
