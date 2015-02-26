<?hh // strict
namespace Titon\Io\Writer;

use Titon\Io\Writer\PhpWriter;
use Titon\Test\TestCase;

class PhpWriterTest extends TestCase {

    protected function setUp(): void {
        parent::setUp();

        $this->setupVFS();
        $this->vfs->createDirectory('/writer');
    }

    public function testWriteResource(): void {
        $path = $this->vfs->path('/writer/php.php');

        $writer = new PhpWriter($path, true);
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

        $expected = <<<PHP
<?php return [
    'integer' => 1234567890,
    'number' => '1234567890',
    'string' => 'abcdefg',
    'false' => false,
    'true' => true,
    'null' => NULL,
    'zero' => 0,
    'map' => [
        'one' => true,
        'two' => false,
    ],
    'map-empty' => [
    ],
    'list' => [
        0 => 1,
        1 => 2,
        2 => 3,
    ],
    'list-empty' => [
    ],
];
PHP;

        $this->assertEquals($expected, str_replace("\t", '    ', file_get_contents($path)));
    }

}
