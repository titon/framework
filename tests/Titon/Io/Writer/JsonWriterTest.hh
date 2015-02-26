<?hh // strict
namespace Titon\Io\Writer;

use Titon\Io\Writer\JsonWriter;
use Titon\Test\TestCase;

class JsonWriterTest extends TestCase {

    protected function setUp(): void {
        parent::setUp();

        $this->setupVFS();
        $this->vfs->createDirectory('/writer');
    }

    public function testWriteResource(): void {
        $path = $this->vfs->path('/writer/json.json');

        $writer = new JsonWriter($path, true);
        $writer->writeResource(Map {
            'integer' => 1234567890,
            'number' => '1234567890',
            'string' => 'abcdefg',
            'false' => false,
            'true' => true,
            'null' => null,
            'zero' => 0,
            'map' => Map {
                'one' => true,
                'two' => false
            },
            'map-empty' => Map {},
            'list' => Vector {1, 2, 3},
            'list-empty' => Vector {}
        });

        $expected = <<<JSON
{
    "integer": 1234567890,
    "number": "1234567890",
    "string": "abcdefg",
    "false": false,
    "true": true,
    "null": null,
    "zero": 0,
    "map": {
        "one": true,
        "two": false
    },
    "map-empty": {},
    "list": [
        1,
        2,
        3
    ],
    "list-empty": []
}
JSON;

        $this->assertEquals($expected, file_get_contents($path));
    }

}
