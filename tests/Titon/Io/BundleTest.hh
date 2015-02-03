<?hh
namespace Titon\Io;

use Titon\Io\Bundle\ResourceBundle;
use Titon\Io\Reader\JsonReader;
use Titon\Io\Reader\PhpReader;
use Titon\Test\TestCase;

/**
 * @property \Titon\Io\Bundle $object
 */
class BundleTest extends TestCase {

    protected function setUp() {
        parent::setUp();

        $this->object = new ResourceBundle();

        $this->setupVFS();
        $this->vfs->createDirectory('/bundle/');
    }

    public function testAddPath() {
        $this->assertEquals(Map {}, $this->object->getPaths());

        $this->object->addPath('foo', '/some/path');

        $this->assertEquals(Map {
            'foo' => Vector {'/some/path/'}
        }, $this->object->getPaths());
    }

    public function testAddPaths() {
        $this->assertEquals(Map {}, $this->object->getPaths());

        $this->object->addPaths('foo', Vector {'/some/path', '/another/path'});
        $this->object->addPaths('bar', Vector {'/one/more/path'});

        $this->assertEquals(Map {
            'foo' => Vector {'/some/path/', '/another/path/'},
            'bar' => Vector {'/one/more/path/'}
        }, $this->object->getPaths());
    }

    public function testAddReader() {
        $this->assertEquals(Map {}, $this->object->getReaders());

        $reader = new PhpReader();
        $this->object->addReader($reader);

        $this->assertEquals(Map {'php' => $reader}, $this->object->getReaders());
    }

    public function testGetContents() {
        $this->object->addPath('test', TEMP_DIR . '/io');

        $paths = $this->object->getContents('test');
        sort($paths);

        $this->assertEquals(Vector {
            TEMP_DIR . '/io/ini.ini',
            TEMP_DIR . '/io/json.json',
            TEMP_DIR . '/io/php.php',
            TEMP_DIR . '/io/po.po',
            TEMP_DIR . '/io/xml.xml',
            TEMP_DIR . '/io/yaml.yaml'
        }, $paths);
    }

    public function testGetDomains() {
        $this->assertEquals(Vector {}, $this->object->getDomains());

        $this->object->addPath('foo', '/some/path');
        $this->object->addPath('bar', '/another/path');

        $this->assertEquals(Vector {'foo', 'bar'}, $this->object->getDomains());
    }

    public function testGetDomainPaths() {
        $this->object->addPaths('foo', Vector {'/some/path', '/another/path'});
        $this->object->addPaths('bar', Vector {'/one/more/path'});

        $this->assertEquals(Vector {'/some/path/', '/another/path/'}, $this->object->getDomainPaths('foo'));
        $this->assertEquals(Vector {'/one/more/path/'}, $this->object->getDomainPaths('bar'));
    }

    /**
     * @expectedException \Titon\Io\Exception\MissingDomainException
     */
    public function testGetDomainPathsErrorsOnMissing() {
        $this->object->getDomainPaths('baz');
    }

    public function testLoadResource() {
        $this->vfs->createDirectory('/bundle/foo');
        $this->vfs->createFile('/bundle/foo/test.php', '<?php return ["foo" => "bar"];');

        $this->object->addPath('foo', $this->vfs->path('/bundle/foo/'));
        $this->object->addReader(new PhpReader());

        $this->assertEquals(Map {'foo' => 'bar'}, $this->object->loadResource('foo', 'test'));
    }

    public function testLoadResourceFromMultiplePaths() {
        $this->vfs->createStructure([
            '/bundle/foo1' => [
                'test.php' => '<?php return ["foo" => "bar"];'
            ],
            '/bundle/foo2' => [
                'test.php' => '<?php return ["foo" => "baz", "qux" => 123];'
            ]
        ]);

        $this->object->addPaths('foo', Vector {$this->vfs->path('/bundle/foo1/'), $this->vfs->path('/bundle/foo2/')});
        $this->object->addReader(new PhpReader());

        $this->assertEquals(Map {
            'foo' => 'baz',
            'qux' => 123
        }, $this->object->loadResource('foo', 'test'));
    }

    public function testLoadResourceUsingMultipleReaders() {
        $this->vfs->createStructure([
            '/bundle/foo1' => [
                'test.php' => '<?php return ["foo" => "bar"];'
            ],
            '/bundle/foo2' => [
                'test.json' => '{"foo": "baz", "qux": 123}'
            ]
        ]);

        $this->object->addPaths('foo', Vector {$this->vfs->path('/bundle/foo1/'), $this->vfs->path('/bundle/foo2/')});
        $this->object->addReader(new PhpReader());
        $this->object->addReader(new JsonReader());

        $this->assertEquals(Map {
            'foo' => 'baz',
            'qux' => 123
        }, $this->object->loadResource('foo', 'test'));
    }

}
