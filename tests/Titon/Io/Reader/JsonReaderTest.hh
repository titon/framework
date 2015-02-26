<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Io\Reader;

use Titon\Test\TestCase;

class JsonReaderTest extends TestCase {

    public function testReadResource(): void {
        $reader = new JsonReader(TEMP_DIR . '/io/json.json');

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
            'list-empty' => Map {} // Empty arrays are turned into maps
        }, $reader->readResource());
    }

}
