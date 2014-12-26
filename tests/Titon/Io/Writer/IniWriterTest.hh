<?hh
namespace Titon\Io\Writer;

use Titon\Io\Writer\IniWriter;
use Titon\Test\TestCase;

class IniWriterTest extends TestCase {

    protected function setUp() {
        parent::setUp();

        $this->setupVFS();
        $this->vfs->createDirectory('/writer');
    }

    public function testWriteResource() {
        $path = $this->vfs->path('/writer/ini.ini');

        $writer = new IniWriter($path, true);
        $writer->writeResource(Map {
            'integer' => 1234567890,
            'number' => '1234567890',
            'string' => 'abcdefg',
            'boolean' => true,
            'zero' => 0,
            'map' => Map {
                'one' => 'true',
                'two' => 'false',
            },
            'map-empty' => Map {}
        });

        $expected = <<<INI
integer = 1234567890
number = 1234567890
string = "abcdefg"
boolean = 1
zero = 0

[map]
one = 1
two = 0

[map-empty]

INI;

        $this->assertEquals($expected, file_get_contents($path));
    }

}