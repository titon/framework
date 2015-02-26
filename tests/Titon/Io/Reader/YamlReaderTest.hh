<?hh // strict
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Io\Reader;

use Titon\Test\TestCase;

class YamlReaderTest extends TestCase {

    public function testReadResource(): void {
        if (!extension_loaded('yaml')) {
            $this->markTestSkipped('YAML extension must be installed to use the YamlReader');
        }

        $reader = new YamlReader(TEMP_DIR . '/io/yaml.yaml');

        $this->assertMapsEqual(Map {
            'integer' => 1234567890,
            'number' => '1234567890',
            'string' => 'abcdefg',
            'true_' => true,
            'false_' => false,
            'null_' => null,
            'zero' => 0,
            'map' => Map {
                'one' => true,
                'two' => false,
            },
            'list' => Vector {'foo', 'bar'},
        }, $reader->readResource());
    }

}
