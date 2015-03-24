<?hh
namespace Titon\Environment;

use Titon\Test\TestCase;

/**
 * @property \Titon\Environment\Detector $object
 */
class DetectorTest extends TestCase {

    protected function setUp(): void {
        parent::setUp();

        $this->object = new Detector(TEMP_DIR . '/environment');
        $this->object->initialize();
    }

    public function testGetEnvironment(): void {
        $this->assertEquals('testing', $this->object->getEnvironment());
    }

    public function testGetLookupPath(): void {
        $this->assertEquals(TEMP_DIR . '/environment/', $this->object->getLookupPath());
    }

    public function testGetVariable(): void {
        $this->assertEquals('qux', $this->object->getVariable('FOO'));
        $this->assertEquals('string without quotes', $this->object->getVariable('STRING_NOQ'));
        $this->assertEquals('Woah, qux and testing', $this->object->getVariable('INTERPOLATE'));
        $this->assertEquals('', $this->object->getVariable('MISSING_KEY'));
    }

    public function testGetVariables(): void {
        $this->assertEquals(ImmMap {
            'APP_ENV' => 'testing',
            'FOO' => 'qux',
            'BAR' => 'bar',
            'BAZ' => 'baz',
            'STRING' => 'string',
            'STRING_NOQ' => 'string without quotes',
            'STRING_SQ' => 'string with single quotes',
            'STRING_DQ' => 'string with double quotes',
            'INTEGER' => '123456',
            'BOOL_TRUE' => '1',
            'BOOL_FALSE' => '',
            'INTERPOLATE' => 'Woah, qux and testing',
        }, $this->object->getVariables());
    }

    public function testIs(): void {
        $this->assertTrue($this->object->is('testing'));
        $this->assertFalse($this->object->is('prod'));
    }

    /**
     * @expectedException \Titon\Environment\Exception\NoDotEnvConfigException
     */
    public function testMissingEnvFileThrowsException() {
        $detector = new Detector(TEMP_DIR . '/environment/fake/path');
        $detector->initialize();
    }

}
