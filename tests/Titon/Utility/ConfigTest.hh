<?hh
namespace Titon\Utility;

use Titon\Io\Reader\PhpReader;
use Titon\Test\TestCase;

class ConfigTest extends TestCase {

    protected $app = Map {
        'name' => 'Titon',
        'salt' => '66c63d989368170aff46040ab2353923',
        'seed' => 'nsdASDn7012dn1dsjSa',
        'encoding' => 'UTF-8'
    };

    protected $debug = Map {
        'level' => 2,
        'email' => ''
    };

    protected $test = Map {
        'integer' => 1234567890,
        'number' => '1234567890',
        'string' => 'abcdefg',
        'vector' => Vector {},
        'array' => [1, 2, 3],
        'false' => false,
        'true' => true,
        'null' => null,
        'zero' => 0
    };

    protected function setUp() {
        parent::setUp();

        $this->setupVFS();

        Config::set('app', $this->app);
        Config::set('debug', $this->debug);
        Config::set('test', $this->test);
    }

    public function testAdd() {
        $this->assertEquals('Titon', Config::get('app.name'));

        Config::add('app.name', 'Framework');
        $this->assertEquals(Vector {'Titon', 'Framework'}, Config::get('app.name'));

        Config::add('app.foobar', 'Titon');
        $this->assertEquals(Vector {'Titon'}, Config::get('app.foobar'));
    }

    public function testAll() {
        $this->assertEquals(Map {
            'app' => $this->app,
            'debug' => $this->debug,
            'test' => $this->test
        }, Config::all());
    }

    public function testEncoding() {
        $this->assertEquals(Config::encoding(), 'UTF-8');

        Config::set('app.encoding', 'UTF-16');
        $this->assertEquals(Config::encoding(), 'UTF-16');

        Config::set('app.encoding', '');
        $this->assertEquals(Config::encoding(), 'UTF-8');
    }

    public function testFlush() {
        $this->assertEquals(Map {
            'app' => $this->app,
            'debug' => $this->debug,
            'test' => $this->test
        }, Config::all());

        Config::flush();

        $this->assertEquals(Map {}, Config::all());
    }

    public function testGet() {
        $this->assertEquals(Config::get('app.name'), $this->app['name']);
        $this->assertEquals(Config::get('app.seed'), $this->app['seed']);

        $this->assertEquals(Config::get('debug'), $this->debug);
        $this->assertEquals(Config::get('debug.level'), $this->debug['level']);

        $this->assertTrue(is_integer(Config::get('test.integer')));
        $this->assertTrue(is_numeric(Config::get('test.number')));
        $this->assertTrue(is_string(Config::get('test.string')));
        $this->assertTrue(empty(Config::get('test.vector')));
        $this->assertTrue(is_array(Config::get('test.array')));

        $this->assertEquals([1, 2, 3], Config::get('test.array'));
        $this->assertEquals(false, Config::get('test.false'));
        $this->assertEquals(true, Config::get('test.true'));
        $this->assertEquals(0, Config::get('test.zero'));
        $this->assertEquals(null, Config::get('test.fakeKey'));

        $this->assertEquals(Config::get('test.string'), $this->test['string']);

        // Default
        $this->assertEquals('bar', Config::get('app.foo', 'bar'));
        Config::set('app.foo', 'baz');
        $this->assertEquals('baz', Config::get('app.foo', 'bar'));
    }

    public function testHas() {
        $this->assertTrue(Config::has('app.salt'));
        $this->assertTrue(Config::has('debug.email'));
        $this->assertTrue(Config::has('test.number'));
        $this->assertTrue(Config::has('test.true'));
        $this->assertTrue(Config::has('test.false'));
        $this->assertTrue(Config::has('test.zero'));

        $this->assertFalse(Config::has('app.id'));
        $this->assertFalse(Config::has('debug.id'));
        $this->assertFalse(Config::has('test.fakeKey'));
        $this->assertFalse(Config::has('test.deep.deep.deep.deep.array'));
    }

    public function testLoad() {
        $data = <<<CFG
<?php

return [
    'integer' => 1234567890,
    'number' => '1234567890',
    'string' => 'abcdefg',
    'vector' => [],
    'array' => [1, 2, 3],
    'false' => false,
    'true' => true,
    'null' => null,
    'zero' => 0
];
CFG;


        $this->vfs->createFile('/config.php', $data);

        $reader = new PhpReader($this->vfs->path('/config.php'));

        Config::load('Php', $reader);
        $this->assertTrue(isset(Config::all()['Php']));

        $this->assertEquals(Map {
            'integer' => 1234567890,
            'number' => '1234567890',
            'string' => 'abcdefg',
            'vector' => Map {},
            'array' => Vector {1, 2, 3},
            'false' => false,
            'true' => true,
            'null' => null,
            'zero' => 0
        }, Config::get('Php'));
    }

    public function testName() {
        $this->assertEquals(Config::name(), $this->app['name']);

        Config::set('app.name', 'TestName');
        $this->assertEquals(Config::name(), 'TestName');

        Config::set('app.name', '');
        $this->assertEquals(Config::name(), '');
    }

    public function testRemove() {
        $this->assertTrue(Config::has('app.salt'));
        $this->assertTrue(Config::has('debug.email'));

        Config::remove('debug');

        $this->assertTrue(Config::has('app.salt'));
        $this->assertFalse(Config::has('debug.email'));
    }

    public function testSalt() {
        $this->assertEquals(Config::salt(), $this->app['salt']);

        Config::set('app.salt', md5('TestSalt'));
        $this->assertEquals(Config::salt(), md5('TestSalt'));

        Config::set('app.salt', '');
        $this->assertEquals(Config::salt(), '');
    }

    public function testSet() {
        Config::set('Set.level1', 1);
        $this->assertEquals(Config::get('Set.level1'), 1);

        Config::set('Set.level2.level2', 2);
        $this->assertEquals(Config::get('Set.level2.level2'), 2);

        Config::set('Set.level3.level3.level3', 3);
        $this->assertEquals(Config::get('Set.level3.level3.level3'), 3);

        Config::set('Set.level4.level4.level4.level4', 4);
        $this->assertEquals(Config::get('Set.level4.level4.level4.level4'), 4);
        $this->assertFalse(Config::get('Set.level4.level4') === 'falsey');

        Config::set('Set.level4.array', Map {'key' => 'value'});
        $this->assertEquals(Config::get('Set.level4.array'), Map {'key' => 'value'});
        $this->assertEquals(Config::get('Set.level4.array.key'), 'value');

        Config::set('Set.level4', 'Flattened!');
        $this->assertEquals(Config::get('Set.level4'), 'Flattened!');
        $this->assertEquals(Config::get('Set.level4.level4.level4.level4'), null);
    }

}