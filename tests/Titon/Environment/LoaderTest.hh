<?hh
namespace Titon\Environment;

use Titon\Test\TestCase;

/**
 * @property \Titon\Environment\Loader $object
 */
class LoaderTest extends TestCase {

    protected function setUp(): void {
        parent::setUp();

        $this->object = new Loader(TEMP_DIR . '/environment/.env');
    }

    public function testVariablesAreExtracted(): void {
        $this->assertEquals(Map {
            'APP_ENV' => 'testing',
            'FOO' => 'foo',
            'BAR' => 'bar',
            'BAZ' => 'baz',
            'STRING' => 'string',
            'STRING_NOQ' => 'string without quotes',
            'STRING_SQ' => 'string with single quotes',
            'STRING_DQ' => 'string with double quotes',
            'INTEGER' => '123456',
            'BOOL_TRUE' => '1',
            'BOOL_FALSE' => '',
        }, $this->object->getVariables());
    }

    public function testVariableIsSetWithPutenv() {
        $this->assertEquals('foo', getenv('FOO'));
    }

    public function testVariableInterpolation() {
        $loader = new Loader(TEMP_DIR . '/environment/.env.testing');

        $this->assertEquals(Map {
            'APP_ENV' => 'shouldNotOverride',
            'FOO' => 'qux',
            'INTERPOLATE' => 'Woah, qux and shouldNotOverride',
        }, $loader->getVariables());
    }

    public function testVariablesAreMerged() {
        $loader = new Loader(TEMP_DIR . '/environment/.env.testing', $this->object->getVariables());

        $this->assertEquals(Map {
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
        }, $loader->getVariables());
    }

}
