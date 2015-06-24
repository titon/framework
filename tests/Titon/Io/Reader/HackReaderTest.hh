<?hh
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Io\Reader;

use Titon\Test\TestCase;

class HackReaderTest extends TestCase {

    public function testReadResource(): void {
        $reader = new HackReader(TEMP_DIR . '/io/hack.hh');

        $this->assertMapsEqual(Map {
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
        }, $reader->readResource());
    }

}
