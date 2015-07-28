<?hh // partial
// Because it extends a non-strict PHP file.
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Test;

use Titon\Utility\Config;
use Titon\Utility\Registry;
use Titon\Utility\State\Cookie;
use Titon\Utility\State\Env;
use Titon\Utility\State\Files;
use Titon\Utility\State\Get;
use Titon\Utility\State\Post;
use Titon\Utility\State\Request;
use Titon\Utility\State\Server;
use Titon\Utility\State\Session;
use VirtualFileSystem\FileSystem;

type FixtureMap = Map<string, Fixture>;

/**
 * Extends the PHPUnit TestCase class with more functionality.
 */
class TestCase extends \PHPUnit_Framework_TestCase {

    const DATE_RFC3339_REGEX = '\d{4}\-\d{2}\-\d{2}T\d{2}:\d{2}:\d{2}\+\d{2}:\d{2}';

    /**
     * List of loaded fixtures.
     *
     * @var \Titon\Test\FixtureMap
     */
    protected FixtureMap $fixtures = Map {};

    /**
     * Virtual file system.
     *
     * @property \VirtualFileSystem\FileSystem $vfs
     */
    protected ?FileSystem $vfs;

    /**
     * Setup state.
     */
    protected function setUp(): void {
        parent::setUp();

        $_COOKIE = [];
        $_FILES = [];
        $_GET = [];
        $_POST = [];
        $_REQUEST = [];
        $_SESSION = [];
        $_SERVER = array_merge($_SERVER, [
            'HTTP_HOST' => 'localhost',
            'DOCUMENT_ROOT' => '/root',
            'SCRIPT_FILENAME' => '/root/index.php',
            'PHP_SELF' => '/index.php',
            'REQUEST_URI' => '/',
            'REQUEST_METHOD' => 'GET',
            'SERVER_ADDR' => '',
            'HTTP_USER_AGENT' => 'titon',
            'PATH_INFO' => '/',
            'SERVER_PORT' => 80,
            'HTTPS' => 'off'
        ]);

        // Initialize globals
        Cookie::initialize($_COOKIE);
        Env::initialize($_ENV);
        Files::initialize($_FILES);
        Get::initialize($_GET);
        Post::initialize($_POST);
        Request::initialize($_REQUEST);
        Server::initialize($_SERVER);
        Session::initialize($_SESSION);
    }

    /**
     * Unload fixtures, remove virtual streams, and clear registry.
     */
    protected function tearDown(): void {
        parent::tearDown();

        //$this->unloadFixtures();
        $this->destroyVFS();

        // Flush static helpers
        Config::flush();
        Registry::flush();
    }

    /**
     * Strip new lines and tabs to test template files easily.
     *
     * @param string $string
     * @return string
     */
    public function clean(string $string): string {
        return str_replace(["\t", "\r", "\n"], '', $string);
    }

    /**
     * Clean new lines by removing carriage returns.
     *
     * @param string $string
     * @return string
     */
    public function nl(string $string): string {
        return str_replace("\r", '', $string);
    }

    /**
     * Get a fixture by name.
     *
     * @param string $name
     * @return \Titon\Test\Fixture
     * @throws \Exception
     */
//    public function getFixture(string $name): Fixture {
//        if ($this->getFixtures()->contains($name)) {
//            return $this->fixtures[$name];
//        }
//
//        throw new \Exception(sprintf('Fixture %s does not exist', $name));
//    }

    /**
     * Return all fixtures.
     *
     * @return \Titon\Test\FixtureMap
     */
//    public function getFixtures(): FixtureMap {
//        return $this->fixtures;
//    }

    /**
     * Load fixtures and generate database records.
     *
     * @return $this
     */
//    public function loadFixtures(): this {
//        $fixtures = func_get_args();
//
//        if (is_array($fixtures[0])) {
//            $fixtures = $fixtures[0];
//        }
//
//        foreach ($fixtures as $fixture) {
//            $className = sprintf('Titon\Test\Fixture\%sFixture', $fixture);
//
//            /** @var \Titon\Test\Fixture $object */
//            $object = new $className();
//            $object->createTable();
//            $object->insertRecords();
//
//            $this->fixtures[$fixture] = $object;
//        }
//
//        return $this;
//    }

    /**
     * Delete the fixture and drop the related table.
     *
     * @return $this
     */
//    public function unloadFixtures(): this {
//        foreach ($this->getFixtures() as $name => $fixture) {
//            $fixture->dropTable();
//        }
//
//        $this->getFixtures()->clear();
//
//        return $this;
//    }

    /**
     * Instantiate a new virtual file system.
     *
     * @return \VirtualFileSystem\FileSystem
     */
    public function setupVFS(): FileSystem {
        return $this->vfs = new FileSystem();
    }

    /**
     * Destroy the virtual file system.
     *
     * @return $this
     */
    public function destroyVFS(): this {
        $this->vfs = null;

        return $this;
    }

    /**
     * Return a virtual file system, or create it if it doesn't exist.
     *
     * @return \VirtualFileSystem\FileSystem
     */
    public function vfs(): FileSystem {
        return ($this->vfs) ? $this->vfs : $this->setupVFS();
    }

    /**
     * Assert that two maps are equal, disregarding the order.
     *
     * @param Map<Tk, Tv> $expected
     * @param Map<Tk, Tv> $actual
     */
    public function assertMapsEqual<Tk, Tv>(Map<Tk, Tv> $expected, Map<Tk, Tv> $actual): void {
        ksort($actual);
        ksort($expected);

        $this->assertEquals($expected, $actual);
    }

    /**
     * Assert that two vectors are equal, disregarding the order.
     *
     * @param Vector<Tv> $expected
     * @param Vector<Tv> $actual
     */
    public function assertVectorsEqual<Tv>(Vector<Tv> $expected, Vector<Tv> $actual): void {
        sort($actual);
        sort($expected);

        $this->assertEquals($expected, $actual);
    }

}
