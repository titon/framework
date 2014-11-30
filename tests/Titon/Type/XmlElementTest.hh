<?hh
namespace Titon\Type;

use Titon\Test\TestCase;

/**
 * @property \Titon\Type\XmlElement $object
 */
class XmlElementTest extends TestCase {

    protected function setUp() {
        parent::setUp();

        $this->object = new XmlElement('root', Map {'foo' => 'bar'});
    }

    public function testConstruct() {
        $this->assertEquals('root', $this->object->getName());
        $this->assertEquals(Map {'foo' => 'bar'}, $this->object->getAttributes());
    }

    public function testAddChild() {
        $boy = new XmlElement('boy');
        $girl = new XmlElement('girl');

        $this->assertEquals(Vector {}, $this->object->getChildren());

        $this->object->addChild($boy);
        $this->object->addChild($girl);

        $this->assertEquals(Vector {$boy, $girl}, $this->object->getChildren());
    }

    public function testAddChildren() {
        $boy = new XmlElement('boy');
        $girl = new XmlElement('girl');

        $this->assertEquals(Vector {}, $this->object->getChildren());

        $this->object->addChildren(Vector {$boy, $girl});

        $this->assertEquals(Vector {$boy, $girl}, $this->object->getChildren());
    }

    public function testCount() {
        $boy = new XmlElement('boy');
        $girl = new XmlElement('girl');

        $this->assertEquals(0, $this->object->count());

        $this->object->addChildren(Vector {$boy, $girl});

        $this->assertEquals(2, $this->object->count());
    }

    public function testGetAttribute() {
        $this->assertEquals('', $this->object->getAttribute('baz'));
        $this->assertEquals('bar', $this->object->getAttribute('foo'));
    }

    public function testGetAttributes() {
        $this->assertEquals(Map {'foo' => 'bar'}, $this->object->getAttributes());
    }

    public function testGetName() {
        $this->assertEquals('root', $this->object->getName());
    }

    public function testGetParent() {
        $child = new XmlElement('child');

        $this->object->addChild($child);

        $this->assertEquals(null, $this->object->getParent());
        $this->assertEquals($this->object, $child->getParent());
    }

    public function testHasAttributes() {
        $child = new XmlElement('child');

        $this->assertFalse($child->hasAttributes());

        $child->setAttribute('foo', 'bar');

        $this->assertTrue($child->hasAttributes());
    }

    public function testHasChildren() {
        $child = new XmlElement('child');

        $this->assertFalse($child->hasChildren());

        $child->addChild(new XmlElement('grandchild'));

        $this->assertTrue($child->hasChildren());
    }

    public function testIsRoot() {
        $child = new XmlElement('child');

        $this->object->addChild($child);

        $this->assertTrue($this->object->isRoot());
        $this->assertFalse($child->isRoot());
    }

    public function testSetAttribute() {
        $this->assertEquals(Map {'foo' => 'bar'}, $this->object->getAttributes());

        $this->object->setAttribute('foo', 'baz');

        $this->assertEquals(Map {'foo' => 'baz'}, $this->object->getAttributes());
    }

    public function testSetAttributes() {
        $this->assertEquals(Map {'foo' => 'bar'}, $this->object->getAttributes());

        $this->object->setAttributes(Map {
            'foo' => 'baz',
            'bar' => 'qux'
        });

        $this->assertEquals(Map {
            'foo' => 'baz',
            'bar' => 'qux'
        }, $this->object->getAttributes());
    }

    public function testSetName() {
        $this->assertEquals('root', $this->object->getName());

        $this->object->setName('document');

        $this->assertEquals('document', $this->object->getName());
    }

    public function testSetNameInflection() {
        $this->object->setName('123');
        $this->assertEquals('_123', $this->object->getName());

        $this->object->setName('foo bar');
        $this->assertEquals('fooBar', $this->object->getName());
    }

    public function testSetValue() {
        $this->assertEquals('', $this->object->getValue());

        $this->object->setValue('value');

        $this->assertEquals('value', $this->object->getValue());
    }

    public function testSetValueCdata() {
        $this->assertEquals('', $this->object->getValue());

        $this->object->setValue('value', true);

        $this->assertEquals('<![CDATA[' . PHP_EOL . 'value' . PHP_EOL . ']]>', $this->object->getValue());
    }

    public function testToString() {
        $xml = new XmlElement('root');

        $this->assertEquals('<root/>' . PHP_EOL, (string) $xml);
    }

    public function testToStringValue() {
        $xml = new XmlElement('root');
        $xml->setValue('foo');

        $this->assertEquals('<root>foo</root>' . PHP_EOL, (string) $xml);
    }

    public function testToStringAttributes() {
        $xml = new XmlElement('root', Map {'foo' => 'bar'});

        $this->assertEquals('<root foo="bar"/>' . PHP_EOL, (string) $xml);
    }

    public function testToStringAttributesWithQuotes() {
        $xml = new XmlElement('root', Map {'single' => "quo'tes", 'double' => 'quo"tes'});

        $this->assertEquals('<root single="quo&#039;tes" double="quo&quot;tes"/>' . PHP_EOL, (string) $xml);
    }

    public function testToStringAttributesAndValue() {
        $xml = new XmlElement('root', Map {'foo' => 'bar'});
        $xml->setValue('baz');

        $this->assertEquals('<root foo="bar">baz</root>' . PHP_EOL, (string) $xml);
    }

    public function testToStringChildren() {
        $xml = new XmlElement('root');
        $xml->setValue('Will not be to stringed.');

        $boy = new XmlElement('boy', Map {'age' => 15});
        $girl = (new XmlElement('girl'))->setValue('Mary');

        $xml->addChildren(Vector {$boy, $girl});

        $this->assertEquals(
            '<root>' . PHP_EOL .
            '    <boy age="15"/>' . PHP_EOL .
            '    <girl>Mary</girl>' . PHP_EOL .
            '</root>' . PHP_EOL
        , $xml->toString());
    }

    public function testToStringChildrenNoIndent() {
        $xml = new XmlElement('root');
        $xml->setValue('Will not be to stringed.');

        $boy = new XmlElement('boy', Map {'age' => 15});
        $girl = (new XmlElement('girl'))->setValue('Mary');

        $xml->addChildren(Vector {$boy, $girl});

        $this->assertEquals(
            '<root>' . PHP_EOL .
            '<boy age="15"/>' . PHP_EOL .
            '<girl>Mary</girl>' . PHP_EOL .
            '</root>' . PHP_EOL
        , $xml->toString(false));
    }

}