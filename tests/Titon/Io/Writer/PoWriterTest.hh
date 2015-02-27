<?hh
namespace Titon\Io\Writer;

use Titon\Io\Writer\PoWriter;
use Titon\Test\TestCase;

class PoWriterTest extends TestCase {

    protected function setUp(): void {
        parent::setUp();

        $this->vfs()->createDirectory('/writer');
    }

    public function testWriteResource(): void {
        $path = $this->vfs()->path('/writer/po.po');

        $writer = new PoWriter($path, true);
        $writer->writeResource(Map {
            'integer' => '1234567890',
            'string' => 'abcdefg',
            'multiline' => "Multiline message\nMore message here\nAnd more message again",
            'plurals' => Vector {'plural', 'plurals'},
            'This is a string without a value' => '',
            '_comments' => Map {
                'Last-Translator' => 'Miles'
            }
        });

        $expected = <<<PO
msgid ""
msgstr ""
"Project-Id-Version: Titon"
"Last-Translator: Miles"
"Language-Team: "
"Language: "
"MIME-Version: 1.0"
"Content-Type: text/plain; charset=UTF-8"
"Content-Transfer-Encoding: 8bit"
"Plural-Forms: nplurals=2; plural=0;"

msgid "integer"
msgstr "1234567890"

msgid "string"
msgstr "abcdefg"

msgid "multiline"
msgstr "Multiline message"
"More message here"
"And more message again"

msgid "plurals"
msgid_plural "plurals"
msgstr[0] "plural"
msgstr[1] "plurals"

msgid "This is a string without a value"
msgstr ""

PO;

        $this->assertEquals($expected, file_get_contents($path));
    }

}
