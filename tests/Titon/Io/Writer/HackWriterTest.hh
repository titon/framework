<?hh
namespace Titon\Io\Writer;

use Titon\Io\Writer\HackWriter;
use Titon\Test\TestCase;

class HackWriterTest extends TestCase {

    protected function setUp(): void {
        parent::setUp();

        $this->vfs()->createDirectory('/writer');
    }

    public function testWriteResource(): void {
        $path = $this->vfs()->path('/writer/hack.hh');

        $writer = new HackWriter($path, true);
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

        $expected = <<<HACK
<?hh return HH\Map {
    'integer' => 1234567890,
    'number' => '1234567890',
    'string' => 'abcdefg',
    'false' => false,
    'true' => true,
    'null' => NULL,
    'zero' => 0,
    'map' => HH\Map {
        'one' => true,
        'two' => false,
    },
    'map-empty' => HH\Map {
    },
    'list' => HH\Vector {
        1,
        2,
        3,
    },
    'list-empty' => HH\Vector {
    },
};
HACK;

        $this->assertEquals($expected, str_replace("\t", '    ', file_get_contents($path)));
    }

}
