<?hh
/**
 * @copyright   2010-2013, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Io\Reader;

use Titon\Test\TestCase;

class XmlReaderTest extends TestCase {

    public function testReadResource(): void {
        $reader = new XmlReader(TEMP_DIR . '/io/xml.xml');

        $this->assertMapsEqual(Map {
            'integer' => Map {
                '@value' => 1234567890,
                '@attributes' => Map {'type' => 'integer'}
            },
            'number' => Map {
                '@value' => '1234567890'
            },
            'string' => Map {
                '@value' => 'abcdefg',
                '@attributes' => Map {'key' => 'value'}
            },
            'false' => Map {
                '@value' => false,
                '@attributes' => Map {'type' => 'boolean'}
            },
            'true' => Map {
                '@value' => true,
                '@attributes' => Map {'type' => 'boolean'}
            },
            'zero' => Map {
                '@value' => '0',
                '@attributes' => Map {'type' => 'integer'}
            },
            'null' => Map {
                '@value' => null,
                '@attributes' => Map {'type' => 'null'}
            },
            'map' => Map {
                'one' => Map {
                    '@value' => true,
                    '@attributes' => Map {'type' => 'boolean'}
                },
                'two' => Map {
                    '@value' => false,
                    '@attributes' => Map {'type' => 'boolean'}
                },
                '@attributes' => Map {'type' => 'map'}
            },
            'map-empty' => Map {
                '@value' => ''
            },
            'list' => Map {
                'child' => Vector {
                    Map {'@value' => 'Foo'},
                    Map {'@value' => 'Bar'},
                },
                '@attributes' => Map {'type' => 'list'}
            },
            'list-empty' => Map {
                '@value' => ''
            },
            'cdata' => Map {'@value' => 'Testing'}
        }, $reader->readResource());
    }

}
