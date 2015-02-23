<?hh
namespace Titon\Debug;

use Titon\Test\TestCase;
use Titon\Utility\State\Server;
use \DateTime;

class LoggerTest extends TestCase {

    protected function setUp() {
        parent::setUp();

        $this->setupVFS();
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
        $this->assertFileNotExists($this->vfs->path('/logs/notice-' . $date . '.log'));

        $logger->log(Logger::ERROR, 'Message');
        $logger->log(Logger::NOTICE, 'Message');

        $this->assertFileExists($this->vfs->path('/logs/error-' . $date . '.log'));
        $this->assertFileExists($this->vfs->path('/logs/notice-' . $date . '.log'));
    }

    public function testCreateMessage() {
        $this->assertRegExp('/^\[' . self::DATE_RFC3339_REGEX . '\] Message \[\/\]/' . PHP_EOL, Logger::createMessage(Logger::DEBUG, 'Message'));
        $this->assertRegExp('/^\[' . self::DATE_RFC3339_REGEX . '\] Message \[\/custom\/url\]/' . PHP_EOL, Logger::createMessage(Logger::DEBUG, 'Message', ['url' => '/custom/url']));

        $_SERVER['PATH_INFO'] = '/path/url';
        Server::initialize($_SERVER);

        $this->assertRegExp('/^\[' . self::DATE_RFC3339_REGEX . '\] Message \[\/path\/url\]/' . PHP_EOL, Logger::createMessage(Logger::DEBUG, 'Message'));

        unset($_SERVER['PATH_INFO']);
        $_SERVER['REQUEST_URI'] = '/request/url';
        Server::initialize($_SERVER);

        $this->assertRegExp('/^\[' . self::DATE_RFC3339_REGEX . '\] Message \[\/request\/url\]/' . PHP_EOL, Logger::createMessage(Logger::DEBUG, 'Message'));

        unset($_SERVER['REQUEST_URI']);
        Server::initialize($_SERVER);

        $this->assertRegExp('/^\[' . self::DATE_RFC3339_REGEX . '\] Message /' . PHP_EOL, Logger::createMessage(Logger::DEBUG, 'Message'));
    }

}
