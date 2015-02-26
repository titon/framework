<?hh
namespace Titon\Type\Xml;

use Titon\Test\TestCase;

/**
 * @property \Titon\Type\Xml\Element $object
 */
class XmlElementTest extends TestCase {

    protected function setUp(): void {
        parent::setUp();

        $this->object = new Element('root', Map {'foo' => 'bar'});
    }

    public function testConstruct(): void {
        $this->assertEquals('root', $this->object->getName());
        $this->assertEquals(Map {'foo' => 'bar'}, $this->object->getAttributes());
    }

    public function testAddChild(): void {
        $boy = new Element('boy');
        $girl = new Element('girl');

        $this->assertEquals(Vector {}, $this->object->getChildren());

        $this->object->addChild($boy);
        $this->object->addChild($girl);

        $this->assertEquals(Vector {$boy, $girl}, $this->object->getChildren());
    }

    public function testAddChildren(): void {
        $boy = new Element('boy');
        $girl = new Element('girl');

        $this->assertEquals(Vector {}, $this->object->getChildren());

        $this->object->addChildren(Vector {$boy, $girl});

        $this->assertEquals(Vector {$boy, $girl}, $this->object->getChildren());
    }

    public function testCount(): void {
        $boy = new Element('boy');
        $girl = new Element('girl');

        $this->assertEquals(0, $this->object->count());

        $this->object->addChildren(Vector {$boy, $girl});

        $this->assertEquals(2, $this->object->count());
    }

    public function testGetAttribute(): void {
        $this->assertEquals('', $this->object->getAttribute('baz'));
        $this->assertEquals('bar', $this->object->getAttribute('foo'));
    }

    public function testGetAttributes(): void {
        $this->assertEquals(Map {'foo' => 'bar'}, $this->object->getAttributes());
    }

    public function testGetBoxedValue(): void {
        $this->object->setValue(false, true);

        $this->assertEquals('<![CDATA[' . PHP_EOL . 'false' . PHP_EOL . ']]>', $this->object->getValue());
        $this->assertEquals('false', $this->object->getValueWithoutCdata());
        $this->assertEquals(false, $this->object->getBoxedValue());

        $this->object->setValue(0, true);

        $this->assertEquals('<![CDATA[' . PHP_EOL . '0' . PHP_EOL . ']]>', $this->object->getValue());
        $this->assertEquals('0', $this->object->getValueWithoutCdata());
        $this->assertEquals(0, $this->object->getBoxedValue());

        $this->object->setValue(null, true);

        $this->assertEquals('<![CDATA[' . PHP_EOL . 'null' . PHP_EOL . ']]>', $this->object->getValue());
        $this->assertEquals('null', $this->object->getValueWithoutCdata());
        $this->assertEquals(null, $this->object->getBoxedValue());
    }

    public function testGetChild(): void {
        $boy1 = new Element('boy');
        $boy2 = new Element('boy');
        $girl = new Element('girl');

        $this->object->addChildren(Vector {$boy1, $boy2, $girl});

        $this->assertEquals($boy1, $this->object->getChild('boy'));
        $this->assertEquals($girl, $this->object->getChild('girl'));
        $this->assertEquals(null, $this->object->getChild('dog'));
    }

    public function testGetChildrenByName(): void {
        $boy1 = new Element('boy');
        $boy2 = new Element('boy');
        $girl = new Element('girl');

        $this->object->addChildren(Vector {$boy1, $boy2, $girl});

        $this->assertEquals(Vector {$boy1, $boy2}, $this->object->getChildrenByName('boy'));
        $this->assertEquals(Vector {}, $this->object->getChildrenByName('dog'));
    }

    public function testGetDeclaration(): void {
        $this->assertEquals(Map {
            'version' => '1.0',
            'encoding' => 'UTF-8'
        }, $this->object->getDeclaration());
    }

    public function testGetName(): void {
        $this->assertEquals('root', $this->object->getName());
    }

    public function testGetNamespace(): void {
        $this->assertEquals('', $this->object->getNamespace('foo'));

        $this->object->setNamespace('foo', 'baz');

        $this->assertEquals('baz', $this->object->getNamespace('foo'));
    }

    public function testGetNamespaces(): void {
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

    public function testGetNamespaceAttributes(): void {
        $this->object->setAttribute('ns:foo', 'qux');

        $this->assertEquals(Map {'foo' => 'bar', 'ns:foo' => 'qux'}, $this->object->getAttributes());
        $this->assertEquals(Map {'ns:foo' => 'qux'}, $this->object->getNamespaceAttributes('ns'));
    }

    public function testGetNamespaceChildren(): void {
        $boy = new Element('boy');
        $girl = new Element('ns:girl');

        $this->object->addChildren(Vector {$boy, $girl});

        $this->assertEquals(Vector {$boy, $girl}, $this->object->getChildren());
        $this->assertEquals(Vector {$girl}, $this->object->getNamespaceChildren('ns'));
    }

    public function testGetParent(): void {
        $child = new Element('child');

        $this->object->addChild($child);

        $this->assertEquals(null, $this->object->getParent());
        $this->assertEquals($this->object, $child->getParent());
    }

    public function testGetValueWithoutCdata(): void {
        $this->object->setValue('value', true);

        $this->assertEquals('<![CDATA[' . PHP_EOL . 'value' . PHP_EOL . ']]>', $this->object->getValue());
        $this->assertEquals('value', $this->object->getValueWithoutCdata());

        $this->object->setValue(0, true);

        $this->assertEquals('<![CDATA[' . PHP_EOL . '0' . PHP_EOL . ']]>', $this->object->getValue());
        $this->assertEquals('0', $this->object->getValueWithoutCdata());
    }

    public function testHasAttribute(): void {
        $this->assertTrue($this->object->hasAttribute('foo'));
        $this->assertFalse($this->object->hasAttribute('bar'));
    }

    public function testHasAttributes(): void {
        $child = new Element('child');

        $this->assertFalse($child->hasAttributes());

        $child->setAttribute('foo', 'bar');

        $this->assertTrue($child->hasAttributes());
    }

    public function testHasChildren(): void {
        $child = new Element('child');

        $this->assertFalse($child->hasChildren());

        $child->addChild(new Element('grandchild'));

        $this->assertTrue($child->hasChildren());
    }

    public function testHasNamespace(): void {
        $this->assertFalse($this->object->hasNamespace('foo'));

        $this->object->setNamespace('foo', 'bar');

        $this->assertTrue($this->object->hasNamespace('foo'));
    }

    public function testHasNamespaces(): void {
        $this->assertFalse($this->object->hasNamespaces());

        $this->object->setNamespace('foo', 'bar');

        $this->assertTrue($this->object->hasNamespaces());
    }

    public function testIsChild(): void {
        $child = new Element('child');

        $this->object->addChild($child);

        $this->assertFalse($this->object->isChild());
        $this->assertTrue($child->isChild());
    }

    public function testIsRoot(): void {
        $child = new Element('child');

        $this->object->addChild($child);

        $this->assertTrue($this->object->isRoot());
        $this->assertFalse($child->isRoot());
    }

    public function testIterator(): void {
        $boy = new Element('boy');
        $girl = new Element('girl');
        $names = [];

        $this->object->addChildren(Vector {$boy, $girl});

        foreach ($this->object as $child) {
            $names[] = $child->getName();
        }

        $this->assertEquals(['boy', 'girl'], $names);
    }

    public function testSetAttribute(): void {
        $this->assertEquals(Map {'foo' => 'bar'}, $this->object->getAttributes());

        $this->object->setAttribute('foo', 'baz');

        $this->assertEquals(Map {'foo' => 'baz'}, $this->object->getAttributes());

        $this->object->setAttribute('foo', 'baz', 'ns');

        $this->assertEquals(Map {'foo' => 'baz', 'ns:foo' => 'baz'}, $this->object->getAttributes());
    }

    public function testSetAttributes(): void {
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

    public function testSetDeclaration(): void {
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

    public function testSetName(): void {
        $this->assertEquals('root', $this->object->getName());

        $this->object->setName('document');

        $this->assertEquals('document', $this->object->getName());

        $this->object->setName('document', 'ns');

        $this->assertEquals('ns:document', $this->object->getName());
    }

    public function testSetNamespace(): void {
        $this->assertEquals(Map {}, $this->object->getNamespaces());

        $this->object->setNamespace('foo', 'baz');

        $this->assertEquals(Map {'foo' => 'baz'}, $this->object->getNamespaces());
    }

    public function testSetNamespaces(): void {
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

    public function testSetNameInflection(): void {
        $this->object->setName('123');
        $this->assertEquals('_123', $this->object->getName());

        $this->object->setName('foo bar');
        $this->assertEquals('foobar', $this->object->getName());
    }

    public function testSetValue(): void {
        $this->assertEquals('', $this->object->getValue());

        $this->object->setValue('value');

        $this->assertEquals('value', $this->object->getValue());
    }

    public function testSetValueCdata(): void {
        $this->assertEquals('', $this->object->getValue());

        $this->object->setValue('value', true);

        $this->assertEquals('<![CDATA[' . PHP_EOL . 'value' . PHP_EOL . ']]>', $this->object->getValue());
    }

    public function testToMap(): void {
        $this->assertEquals(Map {
            'root' => Map {
                '@attributes' => Map {'foo' => 'bar'},
                '@value' => ''
            }
        }, $this->object->toMap());
    }

    public function testToMapValue(): void {
        $this->object->setValue('foo');

        $this->assertEquals(Map {
            'root' => Map {
                '@attributes' => Map {'foo' => 'bar'},
                '@value' => 'foo'
            }
        }, $this->object->toMap());
    }

    public function testToMapZeroValue(): void {
        $this->object->setValue('0');

        $this->assertEquals(Map {
            'root' => Map {
                '@attributes' => Map {'foo' => 'bar'},
                '@value' => 0
            }
        }, $this->object->toMap());
    }

    public function testToMapChildren(): void {
        $this->object->setValue('foo');

        $this->object->addChildren(Vector {
            new Element('boy'),
            new Element('girl')
        });

        $this->assertEquals(Map {
            'root' => Map {
                '@attributes' => Map {'foo' => 'bar'},
                'boy' => Map {'@value' => ''},
                'girl' => Map {'@value' => ''}
            }
        }, $this->object->toMap());
    }

    public function testToMapChildrenWithSameName(): void {
        $this->object->addChildren(Vector {
            new Element('boy', Map {'a' => '1'}),
            new Element('boy', Map {'b' => '2'}),
            (new Element('boy'))->setValue('3'),
            new Element('girl')
        });

        $this->assertEquals(Map {
            'root' => Map {
                '@attributes' => Map {'foo' => 'bar'},
                'boy' => Vector {
                    Map {'@attributes' => Map {'a' => 1}, '@value' => ''},
                    Map {'@attributes' => Map {'b' => 2}, '@value' => ''},
                    Map {'@value' => '3'}
                },
                'girl' => Map {'@value' => ''}
            }
        }, $this->object->toMap());
    }

    public function testToString(): void {
        $xml = new Element('root');

        $this->assertEquals(
            '<?xml version="1.0" encoding="UTF-8"?>' . PHP_EOL .
            '<root/>' . PHP_EOL
        , (string) $xml);
    }

    public function testToStringValue(): void {
        $xml = new Element('root');
        $xml->setValue('foo');

        $this->assertEquals(
            '<?xml version="1.0" encoding="UTF-8"?>' . PHP_EOL .
            '<root>foo</root>' . PHP_EOL
        , (string) $xml);
    }

    public function testToStringAttributes(): void {
        $xml = new Element('root', Map {'foo' => 'bar'});

        $this->assertEquals(
            '<?xml version="1.0" encoding="UTF-8"?>' . PHP_EOL .
            '<root foo="bar"/>' . PHP_EOL
        , (string) $xml);
    }

    public function testToStringAttributesWithQuotes(): void {
        $xml = new Element('root', Map {'single' => "quo'tes", 'double' => 'quo"tes'});

        $this->assertEquals(
            '<?xml version="1.0" encoding="UTF-8"?>' . PHP_EOL .
            '<root single="quo&#039;tes" double="quo&quot;tes"/>' . PHP_EOL
        , (string) $xml);
    }

    public function testToStringAttributesAndValue(): void {
        $xml = new Element('root', Map {'foo' => 'bar'});
        $xml->setValue('baz');

        $this->assertEquals(
            '<?xml version="1.0" encoding="UTF-8"?>' . PHP_EOL .
            '<root foo="bar">baz</root>' . PHP_EOL
        , (string) $xml);
    }

    public function testToStringChildren(): void {
        $xml = new Element('root');
        $xml->setValue('Will not be to stringed.');

        $boy = new Element('boy', Map {'age' => '15'});
        $girl = (new Element('girl'))->setValue('Mary');

        $xml->addChildren(Vector {$boy, $girl});

        $this->assertEquals(
            '<?xml version="1.0" encoding="UTF-8"?>' . PHP_EOL .
            '<root>' . PHP_EOL .
            '    <boy age="15"/>' . PHP_EOL .
            '    <girl>Mary</girl>' . PHP_EOL .
            '</root>' . PHP_EOL
        , $xml->toString());
    }

    public function testToStringChildrenNoIndent(): void {
        $xml = new Element('root');
        $xml->setValue('Will not be to stringed.');

        $boy = new Element('boy', Map {'age' => '15'});
        $girl = (new Element('girl'))->setValue('Mary');

        $xml->addChildren(Vector {$boy, $girl});

        $this->assertEquals(
            '<?xml version="1.0" encoding="UTF-8"?>' . PHP_EOL .
            '<root>' . PHP_EOL .
            '<boy age="15"/>' . PHP_EOL .
            '<girl>Mary</girl>' . PHP_EOL .
            '</root>' . PHP_EOL
        , $xml->toString(false));
    }

    public function testToStringNamespaces(): void {
        $xml = new Element('n:root');
        $xml->setNamespace('n', 'http://domain.com/url');

        $boy = new Element('n:boy');
        $girl = new Element('n:girl');

        $xml->addChildren(Vector {$boy, $girl});

        $this->assertEquals(
            '<?xml version="1.0" encoding="UTF-8"?>' . PHP_EOL .
            '<n:root xmlns:n="http://domain.com/url">' . PHP_EOL .
            '    <n:boy/>' . PHP_EOL .
            '    <n:girl/>' . PHP_EOL .
            '</n:root>' . PHP_EOL
        , $xml->toString());
    }

    public function testToStringDeclaration(): void {
        $xml = new Element('root');
        $xml->setDeclaration('1.1', 'UTF-16', Map {'standalone' => 'yes'});

        $this->assertEquals(
            '<?xml version="1.1" encoding="UTF-16" standalone="yes"?>' . PHP_EOL .
            '<root/>' . PHP_EOL
        , (string) $xml);
    }

}
