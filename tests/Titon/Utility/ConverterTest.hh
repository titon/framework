<?hh
namespace Titon\Utility;

use Titon\Test\TestCase;
use Titon\Type\Contract\Jsonable;
use Titon\Type\Contract\Mapable;
use Titon\Type\Contract\Vectorable;
use Titon\Type\Contract\Xmlable;
use \SimpleXMLElement;

class ConverterTest extends TestCase {

    protected $array;
    protected $collection;
    protected $object;
    protected $json;
    protected $serialized;
    protected $xml;
    protected $barbarian;

    protected function setUp() {
        parent::setUp();

        $this->array = ['foo' => 'bar'];
        $this->collection = Map {'foo' => 'bar'};
        $this->json = json_encode($this->array);
        $this->serialized = serialize($this->array);
        $this->xml = '<?xml version="1.0" encoding="utf-8"?>' . "\n" . '<root><foo>bar</foo></root>';
        $this->barbarian = file_get_contents(TEMP_DIR . '/utility/barbarian.xml');

        $object = new \stdClass();
        $object->foo = 'bar';
        $this->object = $object;
    }

    public function testAutobox() {
        $this->assertSame(123.45, Converter::autobox('123.45'));
        $this->assertSame(123.45, Converter::autobox(123.45));
        $this->assertSame(123, Converter::autobox('123'));
        $this->assertSame(true, Converter::autobox(true));
        $this->assertSame(false, Converter::autobox('false'));
        $this->assertSame('foo', Converter::autobox('foo'));
    }

    public function testUnbox() {
        $this->assertSame('123.45', Converter::unbox('123.45'));
        $this->assertSame('123.45', Converter::unbox(123.45));
        $this->assertSame('123', Converter::unbox('123'));
        $this->assertSame('true', Converter::unbox(true));
        $this->assertSame('false', Converter::unbox('false'));
        $this->assertSame('foo', Converter::unbox('foo'));
    }

    public function testIs() {
        $this->assertEquals('array', Converter::is($this->array));
        $this->assertEquals('object', Converter::is($this->object));
        $this->assertEquals('json', Converter::is($this->json));
        $this->assertEquals('serialized', Converter::is($this->serialized));
        $this->assertEquals('xml', Converter::is($this->xml));
        $this->assertEquals('map', Converter::is(Map {}));
        $this->assertEquals('vector', Converter::is(Vector {}));
        $this->assertEquals('set', Converter::is(Set {}));
        $this->assertEquals('pair', Converter::is(Pair {1, 2}));
    }

    public function testIsMacro() {
        Converter::macro('isBoolean', function($value) {
            return is_bool($value);
        });

        $this->assertEquals('boolean', Converter::is(true));
    }

    public function testIsResource() {
        $f = fopen('php://input', 'r');

        $this->assertEquals('resource', Converter::is($f));

        fclose($f);
    }

    public function testIsJson() {
        $this->assertFalse((bool) Converter::isJson($this->array));
        $this->assertFalse((bool) Converter::isJson($this->collection));
        $this->assertFalse((bool) Converter::isJson($this->object));
        $this->assertTrue((bool) Converter::isJson($this->json));
        $this->assertFalse((bool) Converter::isJson($this->serialized));
        $this->assertFalse((bool) Converter::isJson($this->xml));
    }

    public function testIsSerialized() {
        $this->assertFalse((bool) Converter::isSerialized($this->array));
        $this->assertFalse((bool) Converter::isSerialized($this->collection));
        $this->assertFalse((bool) Converter::isSerialized($this->object));
        $this->assertFalse((bool) Converter::isSerialized($this->json));
        $this->assertTrue((bool) Converter::isSerialized($this->serialized));
        $this->assertFalse((bool) Converter::isSerialized($this->xml));
    }

    public function testIsXml() {
        $this->assertFalse((bool) Converter::isXml($this->array));
        $this->assertFalse((bool) Converter::isXml($this->collection));
        $this->assertFalse((bool) Converter::isXml($this->object));
        $this->assertFalse((bool) Converter::isXml($this->json));
        $this->assertFalse((bool) Converter::isXml($this->serialized));
        $this->assertTrue((bool) Converter::isXml($this->xml));
    }

    public function testToArray() {
        $map = Map {
            'foo' => 'bar',
            'map' => Map {'baz' => 'qux'},
            'vector' => Vector {1, 2, 3},
            'array' => [1, 2, 'a' => 'b']
        };

        $this->assertEquals([
            'foo' => 'bar',
            'map' => ['baz' => 'qux'],
            'vector' => [1, 2, 3],
            'array' => [1, 2, 'a' => 'b']
        ], Converter::toArray($map));

        $this->assertEquals(['foo'], Converter::toArray('foo'));
    }

    public function testToJson() {
        $this->assertEquals($this->json, Converter::toJson($this->array));
        $this->assertEquals($this->json, Converter::toJson($this->collection));
        $this->assertEquals($this->json, Converter::toJson($this->object));
        $this->assertEquals($this->json, Converter::toJson($this->json));
    }

    public function testToJsonContract() {
        $test = new TypeContractStub($this->collection);
        $this->assertEquals($this->json, Converter::toJson($test));

        $test = new TypeContractStub(['a' => 1]);
        $this->assertEquals('{"a":1}', Converter::toJson($test));
    }

    public function testToMap() {
        $this->assertEquals(Map {}, Converter::toMap(Vector {}));
        $this->assertEquals(Map {0 => 'foo'}, Converter::toMap(Vector {'foo'}));
        $this->assertEquals(Map {}, Converter::toMap([]));
        $this->assertEquals(Map {0 => 'foo'}, Converter::toMap(['foo']));
        $this->assertEquals(Map {0 => 'foo'}, Converter::toMap('foo'));
        $this->assertEquals(Map {0 => 123}, Converter::toMap(123));
        $this->assertEquals(Map {}, Converter::toMap(Map {}));
        $this->assertEquals(Map {'foo' => 'bar'}, Converter::toMap(Map {'foo' => 'bar'}));
        $this->assertEquals(Map {'foo' => 'bar'}, Converter::toMap(new TypeContractStub([])));
    }

    public function testToMapRecursive() {
        $this->assertEquals(Map {
            'depth' => 1,
            'two' => Map {
                'depth' => 2
            }
        }, Converter::toMap([
            'depth' => 1,
            'two' => [
                'depth' => 2
            ]
        ]));
    }

    public function testToSerialize() {
        $this->assertEquals($this->serialized, Converter::toSerialize($this->array));
        $this->assertEquals('K:6:"HH\Map":1:{s:3:"foo";s:3:"bar";}', Converter::toSerialize($this->collection));
        $this->assertEquals('O:8:"stdClass":1:{s:3:"foo";s:3:"bar";}', Converter::toSerialize($this->object));
        $this->assertEquals($this->serialized, Converter::toSerialize($this->serialized));
    }

    public function testToSerializeContract() {
        $test = new TypeContractStub($this->collection);
        $this->assertEquals('C:30:"Titon\Utility\TypeContractStub":37:{K:6:"HH\Map":1:{s:3:"foo";s:3:"bar";}}', Converter::toSerialize($test));
    }

    public function testToXml() {
        $this->assertEquals($this->xml, Converter::toXml($this->array));
        $this->assertEquals($this->xml, Converter::toXml($this->collection));
        $this->assertEquals($this->xml, Converter::toXml($this->xml));
    }

    public function testToXmlContract() {
        $test = new TypeContractStub($this->collection);
        $this->assertEquals($this->xml, Converter::toXml($test));

        $test = new TypeContractStub(['a' => 1]);
        $this->assertEquals('<?xml version="1.0" encoding="utf-8"?>' . "\n" . '<root><a>1</a></root>', Converter::toXml($test));
    }

    /**
     * @expectedException \PHPUnit_Framework_Error_Warning
     */
    public function testToXmlCantStartWithNumbers() {
        $items = [
            $this->createXmlItem(1),
            $this->createXmlItem(2),
            $this->createXmlItem(3)
        ];

        // Without named indices
        $expected  = '<?xml version="1.0" encoding="utf-8"?>' . "\n" . '<root>';
        $expected .= '<0><id>1</id><title>Item #1</title></0>';
        $expected .= '<1><id>2</id><title>Item #2</title></1>';
        $expected .= '<2><id>3</id><title>Item #3</title></2>';
        $expected .= '</root>';

        // XML nodes cant start with numbers
        $this->assertXmlStringEqualsXmlString($expected, Converter::toXml($items));
    }

    public function testToXmlWithNumericIndexes() {
        $numItems = Map {'item' => Vector {
            $this->createXmlItem(1)
        }};

        $expected  = '<?xml version="1.0" encoding="utf-8"?><root>';
        $expected .= '<item><id>1</id><title>Item #1</title></item>';
        $expected .= '</root>';

        $this->assertXmlStringEqualsXmlString($expected, Converter::toXml($numItems));
    }

    public function testToXmlNestedIndexes() {
        $items = Map {'item' => Vector {
            $this->createXmlItem(1),
            $this->createXmlItem(2),
            $this->createXmlItem(3)
        }};

        $expected  = '<?xml version="1.0" encoding="utf-8"?><root>';
        $expected .= '<item><id>1</id><title>Item #1</title></item>';
        $expected .= '<item><id>2</id><title>Item #2</title></item>';
        $expected .= '<item><id>3</id><title>Item #3</title></item>';
        $expected .= '</root>';

        $this->assertXmlStringEqualsXmlString($expected, Converter::toXml($items));
    }

    public function testToXmlNestedDeep() {
        $items = Map {'item' => Vector {
            $this->createXmlItem(1, true),
            $this->createXmlItem(2, true),
            $this->createXmlItem(3, true)
        }};

        $expected  = '<?xml version="1.0" encoding="utf-8"?><root>';
        $expected .= '<item>
            <id>1</id><title>Item #1</title>
            <foo><id>1</id><title>Item #1</title></foo>
            <foo><id>2</id><title>Item #2</title></foo>
            <foo><id>3</id><title>Item #3</title></foo>
        </item>';
        $expected .= '<item>
            <id>2</id><title>Item #2</title>
            <foo><id>1</id><title>Item #1</title></foo>
            <foo><id>2</id><title>Item #2</title></foo>
            <foo><id>3</id><title>Item #3</title></foo>
        </item>';
        $expected .= '<item>
            <id>3</id><title>Item #3</title>
            <foo><id>1</id><title>Item #1</title></foo>
            <foo><id>2</id><title>Item #2</title></foo>
            <foo><id>3</id><title>Item #3</title></foo>
        </item>';
        $expected .= '</root>';

        $this->assertXmlStringEqualsXmlString($expected, Converter::toXml($items));
    }

    public function testToXmlNestedDeepAgain() {
        $items = Map {'item' => Vector {
            $this->createXmlItem(1, 'a'),
            $this->createXmlItem(2, 'b'),
            $this->createXmlItem(3, 'c')
        }};

        $expected  = '<?xml version="1.0" encoding="utf-8"?><items>';
        $expected .= '<item>
            <id>1</id><title>Item #1</title>
            <foo>
                <a><id>1</id><title>Item #1</title></a>
                <a><id>2</id><title>Item #2</title></a>
                <a><id>3</id><title>Item #3</title></a>
            </foo>
        </item>';
        $expected .= '<item>
            <id>2</id><title>Item #2</title>
            <foo>
                <b><id>1</id><title>Item #1</title></b>
                <b><id>2</id><title>Item #2</title></b>
                <b><id>3</id><title>Item #3</title></b>
            </foo>
        </item>';
        $expected .= '<item>
            <id>3</id><title>Item #3</title>
            <foo>
                <c><id>1</id><title>Item #1</title></c>
                <c><id>2</id><title>Item #2</title></c>
                <c><id>3</id><title>Item #3</title></c>
            </foo>
        </item>';
        $expected .= '</items>';

        $this->assertXmlStringEqualsXmlString($expected, Converter::toXml($items, 'items'));
    }

    public function testToXmlArrayOfTypes() {
        $items = Map {'item' => Vector {
            (object) $this->createXmlItem(1),
            $this->createXmlItem(2),
            (object) $this->createXmlItem(3)
        }};

        $expected  = '<?xml version="1.0" encoding="utf-8"?><root>';
        $expected .= '<item><id>1</id><title>Item #1</title></item>';
        $expected .= '<item><id>2</id><title>Item #2</title></item>';
        $expected .= '<item><id>3</id><title>Item #3</title></item>';
        $expected .= '</root>';

        $this->assertXmlStringEqualsXmlString($expected, Converter::toXml($items));
    }

    public function testToVector() {
        $this->assertEquals(Vector {}, Converter::toVector(Vector {}));
        $this->assertEquals(Vector {'foo'}, Converter::toVector(Vector {'foo'}));
        $this->assertEquals(Vector {}, Converter::toVector([]));
        $this->assertEquals(Vector {'foo'}, Converter::toVector(['foo']));
        $this->assertEquals(Vector {'foo'}, Converter::toVector('foo'));
        $this->assertEquals(Vector {123}, Converter::toVector(123));
        $this->assertEquals(Vector {}, Converter::toVector(Map {}));
        $this->assertEquals(Vector {'bar'}, Converter::toVector(Map {'foo' => 'bar'}));
        $this->assertEquals(Vector {'foo'}, Converter::toVector(new TypeContractStub([])));
    }

    public function testToVectorRecursive() {
        $this->assertEquals(Vector {
            'one',
            Vector {'two'}
        }, Converter::toVector([
            'one',
            ['two']
        ]));
    }

    public function testXmlTypeCasting() {
        $data = Map {
            'true' => true,
            'false' => false,
            'null' => null,
            'zero' => 0,
            'empty' => '',
            'float' => 1.50,
            'int' => 666
        };

        $expected  = '<?xml version="1.0" encoding="utf-8"?><root>';
        $expected .= '<true>true</true>';
        $expected .= '<false>false</false>';
        $expected .= '<null></null>';
        $expected .= '<zero>0</zero>';
        $expected .= '<empty></empty>';
        $expected .= '<float>1.5</float>';
        $expected .= '<int>666</int>';
        $expected .= '</root>';

        $this->assertXmlStringEqualsXmlString($expected, Converter::toXml($data));

        $this->assertEquals($data, Converter::xmlToMap(new SimpleXMLElement($expected)));
    }

    public function testXmlToMapNone() {
        $expected = Map {
            'name' => 'Barbarian',
            'life' => 50,
            'mana' => 100,
            'stamina' => 15,
            'vitality' => 20,
            'dexterity' => '',
            'agility' => '',
            'armors' => Map {
                'armor' => Vector {'Helmet', 'Shoulder Plates', 'Breast Plate', 'Greaves', 'Gloves', 'Shield'}
            },
            'weapons' => Map {
                'sword' => Vector {'Broadsword', 'Longsword'},
                'axe' => Vector {'Heavy Axe', 'Double-edged Axe'},
                'polearm' => 'Polearm',
                'mace' => 'Mace'
            },
            'items' => Map {
                'potions' => Map {
                    'potion' => Vector {'Health Potion', 'Mana Potion'}
                },
                'keys' => Map {
                    'chestKey' => 'Chest Key',
                    'bossKey' => 'Boss Key'
                },
                'food' => Vector {'Fruit', 'Bread', 'Vegetables'},
                'scrap' => 'Scrap'
            }
        };

        $this->assertEquals($expected, Converter::xmlToMap(simplexml_load_string($this->barbarian), Converter::XML_NONE));

        $xml = <<<XML
<unit>
    <name>Barbarian</name>
    <life>50</life>
    <mana>100</mana>
    <stamina>15</stamina>
    <vitality>20</vitality>
    <dexterity/>
    <agility/>
    <armors>
        <armor>Helmet</armor>
        <armor>Shoulder Plates</armor>
        <armor>Breast Plate</armor>
        <armor>Greaves</armor>
        <armor>Gloves</armor>
        <armor>Shield</armor>
    </armors>
    <weapons>
        <sword>Broadsword</sword>
        <sword>Longsword</sword>
        <axe>Heavy Axe</axe>
        <axe>Double-edged Axe</axe>
        <polearm>Polearm</polearm>
        <mace>Mace</mace>
    </weapons>
    <items>
        <potions>
            <potion>Health Potion</potion>
            <potion>Mana Potion</potion>
        </potions>
        <keys>
            <chestKey>Chest Key</chestKey>
            <bossKey>Boss Key</bossKey>
        </keys>
        <food>Fruit</food>
        <food>Bread</food>
        <food>Vegetables</food>
        <scrap>Scrap</scrap>
    </items>
</unit>
XML;

        $xmlObject = simplexml_load_string('<?xml version="1.0" encoding="utf-8"?><unit></unit>');
        $this->assertEquals($this->prepareExpectedXml($xml), Converter::buildXml($xmlObject, $expected)->asXML());
    }

    public function testXmlToMapMerge() {
        $expected = Map {
            'name' => 'Barbarian',
            'life' => Map {'value' => 50, 'max' => 150},
            'mana' => Map {'value' => 100, 'max' => 250},
            'stamina' => 15,
            'vitality' => 20,
            'dexterity' => Map {'value' => '', 'evade' => '5%', 'block' => '10%'},
            'agility' => Map {'value' => '', 'turnRate' => '1.25', 'acceleration' => 5},
            'armors' => Map {
                'armor' => Vector {
                    Map {'value' => 'Helmet', 'defense' => 15},
                    Map {'value' => 'Shoulder Plates', 'defense' => 25},
                    Map {'value' => 'Breast Plate', 'defense' => 50},
                    Map {'value' => 'Greaves', 'defense' => 10},
                    Map {'value' => 'Gloves', 'defense' => 10},
                    Map {'value' => 'Shield', 'defense' => 25},
                },
                'items' => 6
            },
            'weapons' => Map {
                'sword' => Vector {
                    Map {'value' => 'Broadsword', 'damage' => 25},
                    Map {'value' => 'Longsword', 'damage' => 30}
                },
                'axe' => Vector {
                    Map {'value' => 'Heavy Axe', 'damage' => 20},
                    Map {'value' => 'Double-edged Axe', 'damage' => 25},
                },
                'polearm' => Map {'value' => 'Polearm', 'damage' => 50, 'range' => 3, 'speed' => 'slow'},
                'mace' => Map {'value' => 'Mace', 'damage' => 15, 'speed' => 'fast'},
                'items' => 6
            },
            'items' => Map {
                'potions' => Map {
                    'potion' => Vector {'Health Potion', 'Mana Potion'}
                },
                'keys' => Map {
                    'chestKey' => 'Chest Key',
                    'bossKey' => 'Boss Key'
                },
                'food' => Vector {'Fruit', 'Bread', 'Vegetables'},
                'scrap' => Map {'value' => 'Scrap', 'count' => 25}
            }
        };

        $this->assertEquals($expected, Converter::xmlToMap(simplexml_load_string($this->barbarian), Converter::XML_MERGE));

        $xml = <<<XML
<unit>
    <name>Barbarian</name>
    <life max="150">50</life>
    <mana max="250">100</mana>
    <stamina>15</stamina>
    <vitality>20</vitality>
    <dexterity evade="5%" block="10%"/>
    <agility turnRate="1.25" acceleration="5"/>
    <armors>
        <armor defense="15">Helmet</armor>
        <armor defense="25">Shoulder Plates</armor>
        <armor defense="50">Breast Plate</armor>
        <armor defense="10">Greaves</armor>
        <armor defense="10">Gloves</armor>
        <armor defense="25">Shield</armor>
        <items>6</items>
    </armors>
    <weapons>
        <sword damage="25">Broadsword</sword>
        <sword damage="30">Longsword</sword>
        <axe damage="20">Heavy Axe</axe>
        <axe damage="25">Double-edged Axe</axe>
        <polearm damage="50" range="3" speed="slow">Polearm</polearm>
        <mace damage="15" speed="fast">Mace</mace>
        <items>6</items>
    </weapons>
    <items>
        <potions>
            <potion>Health Potion</potion>
            <potion>Mana Potion</potion>
        </potions>
        <keys>
            <chestKey>Chest Key</chestKey>
            <bossKey>Boss Key</bossKey>
        </keys>
        <food>Fruit</food>
        <food>Bread</food>
        <food>Vegetables</food>
        <scrap count="25">Scrap</scrap>
    </items>
</unit>
XML;

        $xmlObject = simplexml_load_string('<?xml version="1.0" encoding="utf-8"?><unit></unit>');
        $this->assertEquals($this->prepareExpectedXml($xml), Converter::buildXml($xmlObject, $expected)->asXML());
    }

    public function testXmlToMapGroup() {
        $expected = Map {
            'name' => 'Barbarian',
            'life' => Map {
                'value' => 50,
                'attributes' => Map {'max' => 150}
            },
            'mana' => Map {
                'value' => 100,
                'attributes' => Map {'max' => 250}
            },
            'stamina' => 15,
            'vitality' => 20,
            'dexterity' => Map {
                'value' => '',
                'attributes' => Map {'evade' => '5%', 'block' => '10%'}
            },
            'agility' => Map {
                'value' => '',
                'attributes' => Map {'turnRate' => '1.25', 'acceleration' => 5}
            },
            'armors' => Map {
                'value' => Map {
                    'armor' => Vector {
                        Map {'value' => 'Helmet', 'attributes' => Map {'defense' => 15}},
                        Map {'value' => 'Shoulder Plates', 'attributes' => Map {'defense' => 25}},
                        Map {'value' => 'Breast Plate', 'attributes' => Map {'defense' => 50}},
                        Map {'value' => 'Greaves', 'attributes' => Map {'defense' => 10}},
                        Map {'value' => 'Gloves', 'attributes' => Map {'defense' => 10}},
                        Map {'value' => 'Shield', 'attributes' => Map {'defense' => 25}},
                    },
                },
                'attributes' => Map {'items' => 6}
            },
            'weapons' => Map {
                'value' => Map {
                    'sword' => Vector {
                        Map {'value' => 'Broadsword', 'attributes' => Map {'damage' => 25}},
                        Map {'value' => 'Longsword', 'attributes' => Map {'damage' => 30}}
                    },
                    'axe' => Vector {
                        Map {'value' => 'Heavy Axe', 'attributes' => Map {'damage' => 20}},
                        Map {'value' => 'Double-edged Axe', 'attributes' => Map {'damage' => 25}},
                    },
                    'polearm' => Map {
                        'value' => 'Polearm',
                        'attributes' => Map {'damage' => 50, 'range' => 3, 'speed' => 'slow'}
                    },
                    'mace' => Map {
                        'value' => 'Mace',
                        'attributes' => Map {'damage' => 15, 'speed' => 'fast'}
                    },
                },
                'attributes' => Map {'items' => 6}
            },
            'items' => Map {
                'potions' => Map {
                    'potion' => Vector {'Health Potion', 'Mana Potion'}
                },
                'keys' => Map {
                    'chestKey' => 'Chest Key',
                    'bossKey' => 'Boss Key'
                },
                'food' => Vector {'Fruit', 'Bread', 'Vegetables'},
                'scrap' => Map {
                    'value' => 'Scrap',
                    'attributes' => Map {'count' => 25}
                }
            }
        };

        $this->assertEquals($expected, Converter::xmlToMap(simplexml_load_string($this->barbarian), Converter::XML_GROUP));

        // Unset the value so that it auto-nulls
        unset($expected['agility']['value']);

        $xml = <<<XML
<unit>
    <name>Barbarian</name>
    <life max="150">50</life>
    <mana max="250">100</mana>
    <stamina>15</stamina>
    <vitality>20</vitality>
    <dexterity evade="5%" block="10%"/>
    <agility turnRate="1.25" acceleration="5"/>
    <armors items="6">
        <armor defense="15">Helmet</armor>
        <armor defense="25">Shoulder Plates</armor>
        <armor defense="50">Breast Plate</armor>
        <armor defense="10">Greaves</armor>
        <armor defense="10">Gloves</armor>
        <armor defense="25">Shield</armor>
    </armors>
    <weapons items="6">
        <sword damage="25">Broadsword</sword>
        <sword damage="30">Longsword</sword>
        <axe damage="20">Heavy Axe</axe>
        <axe damage="25">Double-edged Axe</axe>
        <polearm damage="50" range="3" speed="slow">Polearm</polearm>
        <mace damage="15" speed="fast">Mace</mace>
    </weapons>
    <items>
        <potions>
            <potion>Health Potion</potion>
            <potion>Mana Potion</potion>
        </potions>
        <keys>
            <chestKey>Chest Key</chestKey>
            <bossKey>Boss Key</bossKey>
        </keys>
        <food>Fruit</food>
        <food>Bread</food>
        <food>Vegetables</food>
        <scrap count="25">Scrap</scrap>
    </items>
</unit>
XML;

        $xmlObject = simplexml_load_string('<?xml version="1.0" encoding="utf-8"?><unit></unit>');
        $this->assertEquals($this->prepareExpectedXml($xml), Converter::buildXml($xmlObject, $expected)->asXML());
    }

    public function testXmlToMapAttribs() {
        $expected = Map {
            'name' => 'Barbarian',
            'life' => Map {'max' => 150},
            'mana' => Map {'max' => 250},
            'stamina' => 15,
            'vitality' => 20,
            'dexterity' => Map {'evade' => '5%', 'block' => '10%'},
            'agility' => Map {'turnRate' => '1.25', 'acceleration' => 5},
            'armors' => Map {
                'items' => 6
            },
            'weapons' => Map {
                'items' => 6
            },
            'items' => Map {
                'potions' => Map {
                    'potion' => Vector {'Health Potion', 'Mana Potion'}
                },
                'keys' => Map {
                    'chestKey' => 'Chest Key',
                    'bossKey' => 'Boss Key'
                },
                'food' => Vector {'Fruit', 'Bread', 'Vegetables'},
                'scrap' => Map {'count' => 25}
            }
        };

        $this->assertEquals($expected, Converter::xmlToMap(simplexml_load_string($this->barbarian), Converter::XML_ATTRIBS));
    }

    protected function createXmlItem($id, $complex = false) {
        $item = Map {'id' => $id, 'title' => 'Item #' . $id};

        if ($complex) {
            $item['foo'] = Vector {
                $this->createXmlItem(1),
                $this->createXmlItem(2),
                $this->createXmlItem(3)
            };

            if ($complex !== true) {
                $item['foo'] = Map {$complex => $item['foo']};
            }
        }

        return $item;
    }

    protected function prepareExpectedXml($string) {
        return '<?xml version="1.0" encoding="utf-8"?>' . "\n" . str_replace('    ', '', $this->clean($string)) . "\n";
    }

}

class TypeContractStub implements Jsonable, Xmlable, Mapable<string, mixed>, Vectorable<mixed>, \Serializable {

    protected $data;

    public function __construct($data) {
        $this->data = $data;
    }

    public function toMap(): Map<string, mixed> {
        return Map {'foo' => 'bar'};
    }

    public function toJson(int $options = 0): string {
        return json_encode($this->data, $options);
    }

    public function toXml(string $root = 'root'): string {
        return Converter::toXml($this->data, $root);
    }

    public function toVector(): Vector<mixed> {
        return Vector {'foo'};
    }

    public function serialize() {
        return serialize($this->data);
    }

    public function unserialize($data) {
        $this->data = unserialize($data);
    }

}