<?hh
namespace Titon\Debug;

use Titon\Test\TestCase;
use Titon\Utility\State\Server;
use VirtualFileSystem\FileSystem;
use \DateTime;

class LoggerTest extends TestCase {

    protected function setUp() {
        parent::setUp();

        $this->vfs = new FileSystem();
        $this->vfs->createDirectory('/logs/');
    }

    /**
     * @expectedException \Titon\Debug\Exception\InvalidDirectoryException
     */
    public function testErrorOnInvalidDir() {
        new Logger($this->vfs->path('/logs-missing/'));
    }

    /**
     * @expectedException \Titon\Debug\Exception\UnwritableDirectoryException
     */
    public function testErrorOnUnwritableDir() {
        $this->vfs->createDirectory('/logs-unwritable/', false, 0555);

        new Logger($this->vfs->path('/logs-unwritable/'));
    }

    public function testLog() {
        $logger = new Logger($this->vfs->path('/logs/'));
        $date = date('Y-m-d');

        $this->assertFileNotExists($this->vfs->path('/logs/error-' . $date . '.log'));
        $logger->log(Logger::ERROR, 'Message');
        $this->assertFileExists($this->vfs->path('/logs/error-' . $date . '.log'));

        $this->assertFileNotExists($this->vfs->path('/logs/notice-' . $date . '.log'));
        $logger->log(Logger::NOTICE, 'Message');
        $this->assertFileExists($this->vfs->path('/logs/notice-' . $date . '.log'));
    }

    public function testCreateMessage() {
        $this->assertEquals('[' . date(DateTime::RFC3339) . '] Message ' . PHP_EOL, Logger::createMessage(Logger::DEBUG, 'Message'));
        $this->assertEquals('[' . date(DateTime::RFC3339) . '] Message [/custom/url]' . PHP_EOL, Logger::createMessage(Logger::DEBUG, 'Message', ['url' => '/custom/url']));

        $_SERVER['PATH_INFO'] = '/path/url';
        Server::initialize($_SERVER);

        $this->assertEquals('[' . date(DateTime::RFC3339) . '] Message [/path/url]' . PHP_EOL, Logger::createMessage(Logger::DEBUG, 'Message'));

        unset($_SERVER['PATH_INFO']);
        $_SERVER['REQUEST_URI'] = '/request/url';
        Server::initialize($_SERVER);

        $this->assertEquals('[' . date(DateTime::RFC3339) . '] Message [/request/url]' . PHP_EOL, Logger::createMessage(Logger::DEBUG, 'Message'));

        unset($_SERVER['REQUEST_URI']);
        Server::initialize($_SERVER);

        $this->assertEquals('[' . date(DateTime::RFC3339) . '] Message ' . PHP_EOL, Logger::createMessage(Logger::DEBUG, 'Message'));
    }

}