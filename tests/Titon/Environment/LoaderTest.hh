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
            'INTEGER' => 123456,
            'FLOAT' => 123.456,
            'BOOL_TRUE' => true,
            'BOOL_FALSE' => false,
            'LIST' => ImmMap {0 => 'foo', 1 => 'bar', 2 => 'baz'},
            'MAP' => ImmMap {'foo' => 'bar'}
        }, $this->object->getVariables());
    }

    public function testVariableIsSetWithPutenv(): void {
        $this->assertEquals('foo', $this->object->getVariable('FOO'));
        $this->assertEquals('foo', getenv('FOO'));

        // integers
        $this->assertEquals('123456', $this->object->getVariable('INTEGER'));
        $this->assertEquals('123456', getenv('INTEGER'));

        // bools
        $this->assertEquals('1', getenv('BOOL_TRUE'));
        $this->assertEquals('', getenv('BOOL_FALSE'));
    }

    public function testVariableInterpolation(): void {
        $loader = new Loader(TEMP_DIR . '/environment/.env.testing');

        $this->assertEquals(Map {
            'APP_ENV' => 'shouldNotOverride',
            'FOO' => 'qux',
            'INTERPOLATE' => 'Woah, qux and shouldNotOverride',
        }, $loader->getVariables());
    }

    public function testVariablesAreMerged(): void {
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
            'INTEGER' => 123456,
            'FLOAT' => 123.456,
            'BOOL_TRUE' => true,
            'BOOL_FALSE' => false,
            'LIST' => ImmMap {0 => 'foo', 1 => 'bar', 2 => 'baz'},
            'MAP' => ImmMap {'foo' => 'bar'},
            'INTERPOLATE' => 'Woah, qux and testing',
        }, $loader->getVariables());
    }

    public function testLockAndUnlock(): void {
        Loader::lock('FOO');

        $this->assertEquals('foo', $this->object->getVariable('FOO'));
        $this->assertTrue(Loader::isImmutable('FOO'));

        $this->object->addVariable('FOO', 'oof');

        $this->assertEquals('foo', $this->object->getVariable('FOO'));

        Loader::unlock('FOO');

        $this->object->addVariable('FOO', 'oof');
        $this->assertFalse(Loader::isImmutable('FOO'));

        $this->assertEquals('oof', $this->object->getVariable('FOO'));
    }

}
