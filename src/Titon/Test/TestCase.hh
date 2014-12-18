<?hh
/**
 * @copyright   2010-2013, The Titon Project
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

    /**
     * List of loaded fixtures.
     *
     * @type \Titon\Test\FixtureMap
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

        $this->unloadFixtures();

        unset($this->vfs);

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
    public function getFixture(string $name): Fixture {
        if ($this->fixtures->contains($name)) {
            return $this->fixtures->get($name);
        }

        throw new \Exception(sprintf('Fixture %s does not exist', $name));
    }

    /**
     * Return all fixtures.
     *
     * @return \Titon\Test\FixtureMap
     */
    public function getFixtures(): FixtureMap {
        return $this->fixtures;
    }

    /**
     * Load fixtures and generate database records.
     *
     * @return $this
     */
    public function loadFixtures(): this {
        $fixtures = func_get_args();

        if (is_array($fixtures[0])) {
            $fixtures = $fixtures[0];
        }

        foreach ($fixtures as $fixture) {
            $className = sprintf('Titon\Test\Fixture\%sFixture', $fixture);

            /** @type \Titon\Test\Fixture $object */
            $object = new $className();
            $object->createTable();
            $object->insertRecords();

            $this->fixtures[$fixture] = $object;
        }

        return $this;
    }

    /**
     * Delete the fixture and drop the related table.
     *
     * @return $this
     */
    public function unloadFixtures(): this {
        foreach ($this->fixtures as $name => $fixture) {
            $fixture->dropTable();

            unset($this->fixtures[$name]);
        }

        return $this;
    }

    /**
     * Assert that two array values are equal, disregarding the order.
     *
     * @param array $expected
     * @param array $actual
     * @param bool $key
     */
    public function assertArraysEqual(array $expected, array $actual, bool $key = false): void {
        if ($key) {
            ksort($actual);
            ksort($expected);
        } else {
            sort($actual);
            sort($expected);
        }

        $this->assertEquals($expected, $actual);
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