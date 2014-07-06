<?hh
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Test;

use Titon\Test\Fixture;
use Titon\Utility\Config;
use Titon\Utility\Registry;

/**
 * Extends the PHPUnit TestCase class with more functionality.
 */
class TestCase extends \PHPUnit_Framework_TestCase {

    /**
     * List of loaded fixtures.
     *
     * @type Map<string, Fixture>
     */
    protected Map<string, Fixture> $_fixtures = Map {};

    /**
     * Setup state.
     */
    protected function setUp(): void {
        parent::setUp();

        $_GET = [];
        $_POST = [];
        $_COOKIE = [];
        $_SESSION = [];
        $_REQUEST = [];
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
    }

    /**
     * Unload fixtures and clear registry.
     */
    protected function tearDown(): void {
        parent::tearDown();

        $this->unloadFixtures();

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
        if ($this->_fixtures->contains($name)) {
            return $this->_fixtures->get($name);
        }

        throw new \Exception(sprintf('Fixture %s does not exist', $name));
    }

    /**
     * Return all fixtures.
     *
     * @return \Titon\Test\Fixture[]
     */
    public function getFixtures(): Map<string, Fixture> {
        return $this->_fixtures;
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

            $this->_fixtures->set($fixture, $object);
        }

        return $this;
    }

    /**
     * Delete the fixture and drop the related table.
     *
     * @return $this
     */
    public function unloadFixtures(): this {
        foreach ($this->_fixtures as $name => $fixture) {
            $fixture->dropTable();
            $this->_fixtures->remove($name);
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
     * @param Map<mixed, mixed> $expected
     * @param Map<mixed, mixed> $actual
     */
    public function assertMapsEqual(Map<mixed, mixed> $expected, Map<mixed, mixed> $actual): void {
        ksort($actual);
        ksort($expected);

        $this->assertEquals($expected, $actual);
    }

    /**
     * Assert that two vectors are equal, disregarding the order.
     *
     * @param Vector<mixed> $expected
     * @param Vector<mixed> $actual
     */
    public function assertVectorsEqual(Vector<mixed> $expected, Vector<mixed> $actual): void {
        sort($actual);
        sort($expected);

        $this->assertEquals($expected, $actual);
    }

}