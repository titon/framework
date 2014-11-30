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

    public function testGetDeclaration() {
        $this->assertEquals(Map {
            'version' => '1.0',
            'encoding' => 'UTF-8'
        }, $this->object->getDeclaration());
    }

    public function testGetName() {
        $this->assertEquals('root', $this->object->getName());
    }

    public function testGetNamespace() {
        $this->assertEquals('', $this->object->getNamespace('foo'));

        $this->object->setNamespace('foo', 'baz');

        $this->assertEquals('baz', $this->object->getNamespace('foo'));
    }

    public function testGetNamespaces() {
        $this->assertEquals(Map {}, $this->object->getNamespaces());

        $this->object->setNamespaces(Map {
            'foo' => 'bar',
            'baz' => 'qux'
        });

        $this->assertEquals(Map {
            'foo' => 'bar',
            'baz' => 'qux'
        }, $this->object->getNamespaces());
    }

    public function testGetParent() {
        $child = new XmlElement('child');

        $this->object->addChild($child);

        $this->assertEquals(null, $this->object->getParent());
        $this->assertEquals($this->object, $child->getParent());
    }

    public function testHasAttribute() {
        $this->assertTrue($this->object->hasAttribute('foo'));
        $this->assertFalse($this->object->hasAttribute('bar'));
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

    public function testHasNamespace() {
        $this->assertFalse($this->object->hasNamespace('foo'));

        $this->object->setNamespace('foo', 'bar');

        $this->assertTrue($this->object->hasNamespace('foo'));
    }

    public function testHasNamespaces() {
        $this->assertFalse($this->object->hasNamespaces());

        $this->object->setNamespace('foo', 'bar');

        $this->assertTrue($this->object->hasNamespaces());
    }

    public function testIsRoot() {
        $child = new XmlElement('child');

        $this->object->addChild($child);

        $this->assertTrue($this->object->isRoot());
        $this->assertFalse($child->isRoot());
    }

    public function testIterator() {
        $boy = new XmlElement('boy');
        $girl = new XmlElement('girl');
        $names = [];

        $this->object->addChildren(Vector {$boy, $girl});

        foreach ($this->object as $child) {
            $names[] = $child->getName();
        }

        $this->assertEquals(['boy', 'girl'], $names);
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

    public function testSetDeclaration() {
        $this->assertEquals(Map {
            'version' => '1.0',
            'encoding' => 'UTF-8'
        }, $this->object->getDeclaration());

        $this->object->setDeclaration('1.1', 'UTF-8', Map {'standalone' => 'yes'});

        $this->assertEquals(Map {
            'version' => '1.1',
            'encoding' => 'UTF-8',
            'standalone' => 'yes'
        }, $this->object->getDeclaration());
    }

    public function testSetName() {
        $this->assertEquals('root', $this->object->getName());

        $this->object->setName('document');

        $this->assertEquals('document', $this->object->getName());
    }

    public function testSetNamespace() {
        $this->assertEquals(Map {}, $this->object->getNamespaces());

        $this->object->setNamespace('foo', 'baz');

        $this->assertEquals(Map {'foo' => 'baz'}, $this->object->getNamespaces());
    }

    public function testSetNamespaces() {
        $this->assertEquals(Map {}, $this->object->getNamespaces());

        $this->object->setNamespaces(Map {
            'foo' => 'baz',
            'bar' => 'qux'
        });

        $this->assertEquals(Map {
            'foo' => 'baz',
            'bar' => 'qux'
        }, $this->object->getNamespaces());
    }

    public function testSetNameInflection() {
        $this->object->setName('123');
        $this->assertEquals('_123', $this->object->getName());

        $this->object->setName('foo bar');
        $this->assertEquals('foobar', $this->object->getName());
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

        $this->assertEquals(
            '<?xml version="1.0" encoding="UTF-8"?>' . PHP_EOL .
            '<root/>' . PHP_EOL
        , (string) $xml);
    }

    public function testToStringValue() {
        $xml = new XmlElement('root');
        $xml->setValue('foo');

        $this->assertEquals(
            '<?xml version="1.0" encoding="UTF-8"?>' . PHP_EOL .
            '<root>foo</root>' . PHP_EOL
        , (string) $xml);
    }

    public function testToStringAttributes() {
        $xml = new XmlElement('root', Map {'foo' => 'bar'});

        $this->assertEquals(
            '<?xml version="1.0" encoding="UTF-8"?>' . PHP_EOL .
            '<root foo="bar"/>' . PHP_EOL
        , (string) $xml);
    }

    public function testToStringAttributesWithQuotes() {
        $xml = new XmlElement('root', Map {'single' => "quo'tes", 'double' => 'quo"tes'});

        $this->assertEquals(
            '<?xml version="1.0" encoding="UTF-8"?>' . PHP_EOL .
            '<root single="quo&#039;tes" double="quo&quot;tes"/>' . PHP_EOL
        , (string) $xml);
    }

    public function testToStringAttributesAndValue() {
        $xml = new XmlElement('root', Map {'foo' => 'bar'});
        $xml->setValue('baz');

        $this->assertEquals(
            '<?xml version="1.0" encoding="UTF-8"?>' . PHP_EOL .
            '<root foo="bar">baz</root>' . PHP_EOL
        , (string) $xml);
    }

    public function testToStringChildren() {
        $xml = new XmlElement('root');
        $xml->setValue('Will not be to stringed.');

        $boy = new XmlElement('boy', Map {'age' => 15});
        $girl = (new XmlElement('girl'))->setValue('Mary');

        $xml->addChildren(Vector {$boy, $girl});

        $this->assertEquals(
            '<?xml version="1.0" encoding="UTF-8"?>' . PHP_EOL .
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
            '<?xml version="1.0" encoding="UTF-8"?>' . PHP_EOL .
            '<root>' . PHP_EOL .
            '<boy age="15"/>' . PHP_EOL .
            '<girl>Mary</girl>' . PHP_EOL .
            '</root>' . PHP_EOL
        , $xml->toString(false));
    }

    public function testToStringNamespaces() {
        $xml = new XmlElement('n:root');
        $xml->setNamespace('n', 'http://domain.com/url');

        $boy = new XmlElement('n:boy');
        $girl = new XmlElement('n:girl');

        $xml->addChildren(Vector {$boy, $girl});

        $this->assertEquals(
            '<?xml version="1.0" encoding="UTF-8"?>' . PHP_EOL .
            '<n:root xmlns:n="http://domain.com/url">' . PHP_EOL .
            '    <n:boy/>' . PHP_EOL .
            '    <n:girl/>' . PHP_EOL .
            '</n:root>' . PHP_EOL
        , $xml->toString());
    }

    public function testToStringDeclaration() {
        $xml = new XmlElement('root');
        $xml->setDeclaration('1.1', 'UTF-16', Map {'standalone' => 'yes'});

        $this->assertEquals(
            '<?xml version="1.1" encoding="UTF-16" standalone="yes"?>' . PHP_EOL .
            '<root/>' . PHP_EOL
        , (string) $xml);
    }

}