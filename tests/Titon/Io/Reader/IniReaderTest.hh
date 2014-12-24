<?hh
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Io\Reader;

use Titon\Test\TestCase;

class IniReaderTest extends TestCase {

    public function testReadResource() {
        $reader = new IniReader(TEMP_DIR . '/io/ini.ini');

        $this->assertMapsEqual(Map {
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
        }, $reader->readResource());
    }

}