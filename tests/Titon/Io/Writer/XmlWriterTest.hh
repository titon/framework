<?hh
namespace Titon\Io\Writer;

use Titon\Io\Writer\XmlWriter;
use Titon\Test\TestCase;

class XmlWriterTest extends TestCase {

    protected function setUp(): void {
        parent::setUp();

        $this->vfs()->createDirectory('/writer');
    }

    public function testWriteResource(): void {
        $path = $this->vfs()->path('/writer/xml.xml');

        $writer = new XmlWriter($path, true);
        $writer->writeResource(Map {
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
            'cdata' => Map {'@value' => 'Testing', '@cdata' => true}
        }, 'config');

        $expected = <<<XML
<?xml version="1.0" encoding="UTF-8"?>
<config>
    <integer type="integer">1234567890</integer>
    <number>1234567890</number>
    <string key="value">abcdefg</string>
    <false type="boolean">false</false>
    <true type="boolean">true</true>
    <zero type="integer">0</zero>
    <null type="null">null</null>
    <map type="map">
        <one type="boolean">true</one>
        <two type="boolean">false</two>
    </map>
    <map-empty/>
    <list type="list">
        <child>Foo</child>
        <child>Bar</child>
    </list>
    <list-empty/>
    <cdata><![CDATA[
Testing
]]></cdata>
</config>

XML;

        $this->assertEquals($expected, file_get_contents($path));
    }

}
