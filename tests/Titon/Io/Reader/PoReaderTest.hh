<?hh
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Io\Reader;

use Titon\Test\TestCase;

class PoReaderTest extends TestCase {

    public function testReadResource() {
        $reader = new PoReader(TEMP_DIR . '/io/po.po');

        $this->assertMapsEqual(Map {
            'integer' => '1234567890',
            'string' => 'abcdefg',
            'multiline' => "Multiline message\nMore message here\nAnd more message again",
            'plurals' => Vector {'plural', 'plurals'},
            'This is a string without a value' => ''
        }, $reader->readResource());
    }

}