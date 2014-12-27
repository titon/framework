<?hh
namespace Titon\Io\Writer;

use Titon\Test\TestCase;

class YamlWriterTest extends TestCase {

    protected function setUp() {
        parent::setUp();

        if (!extension_loaded('yaml')) {
            $this->markTestSkipped('YAML extension must be installed to use the YamlWriter');
        }

        $this->setupVFS();
        $this->vfs->createDirectory('/writer');
    }

    public function testWriteResource() {
        $path = $this->vfs->path('/writer/yaml.yaml');

        $writer = new YamlWriter($path, true);
        $writer->writeResource(Map {
            'integer' => 1234567890,
            'number' => '1234567890',
            'string' => 'abcdefg',
            'true' => true,
            'false' => false,
            'null' => null,
            'zero' => 0,
            'map' => Map {
                'one' => true,
                'two' => false,
            },
            'list' => Vector {'foo', 'bar'},
        });

        $expected = <<<YAML
integer: 1234567890
number: 1234567890
string: abcdefg
true: true
false: false
null: null
zero: 0
map:
  one: true
  two: false
list:
  - foo
  - bar
YAML;

        $this->assertEquals($expected, file_get_contents($path));
    }

}