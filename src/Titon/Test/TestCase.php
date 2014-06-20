<?php
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Test;

/**
 * Extends the PHPUnit TestCase class with more functionality.
 */
class TestCase extends \PHPUnit_Framework_TestCase {

    /**
     * List of loaded fixtures.
     *
     * @type \Titon\Test\TestFixture[]
     */
    protected $_fixtures = [];

    /**
     * Setup state.
     */
    protected function setUp() {
        parent::setUp();

        $_GET = [];
        $_POST = [];
        $_COOKIE = [];
        $_SESSION = [];
        $_REQUEST = [];
        $_SERVER = [
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
        ];
    }

    /**
     * Unload fixtures.
     */
    protected function tearDown() {
        parent::tearDown();

        $this->unloadFixtures();
    }

    /**
     * Strip new lines and tabs to test template files easily.
     *
     * @param string $string
     * @return string
     */
    public function clean($string) {
        return str_replace(["\t", "\r", "\n"], '', $string);
    }

    /**
     * Clean new lines by removing carriage returns.
     *
     * @param string $string
     * @return string
     */
    public function nl($string) {
        return str_replace("\r", "", $string);
    }

    /**
     * Get a fixture by name.
     *
     * @param string $name
     * @return \Titon\Test\TestFixture
     * @throws \Exception
     */
    public function getFixture($name) {
        if (isset($this->_fixtures[$name])) {
            return $this->_fixtures[$name];
        }

        throw new \Exception(sprintf('Fixture %s does not exist', $name));
    }

    /**
     * Load fixtures and generate database records.
     *
     * @return $this
     */
    public function loadFixtures() {
        $fixtures = func_get_args();

        if (is_array($fixtures[0])) {
            $fixtures = $fixtures[0];
        }

        foreach ($fixtures as $fixture) {
            $className = sprintf('Titon\Test\Fixture\%sFixture', $fixture);

            /** @type \Titon\Test\TestFixture $object */
            $object = new $className();
            $object->createTable();
            $object->insertRecords();

            $this->_fixtures[$fixture] = $object;
        }

        return $this;
    }

    /**
     * Delete the fixture and drop the related table.
     *
     * @return $this
     */
    public function unloadFixtures() {
        foreach ($this->_fixtures as $name => $fixture) {
            $fixture->dropTable();
            unset($this->_fixtures[$name]);
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
    public function assertArraysEqual(array $expected, array $actual, $key = false) {
        if ($key) {
            ksort($actual);
            ksort($expected);
        } else {
            sort($actual);
            sort($expected);
        }

        $this->assertEquals($expected, $actual);
    }

}