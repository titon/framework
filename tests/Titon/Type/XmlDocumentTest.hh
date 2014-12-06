<?hh
namespace Titon\Type;

use Titon\Test\TestCase;

class XmlDocumentTest extends TestCase {

    public function testBox() {
        $this->assertSame(123.45, XmlDocument::box('123.45'));
        $this->assertSame(123.45, XmlDocument::box('123.45'));
        $this->assertSame(123, XmlDocument::box('123'));
        $this->assertSame(true, XmlDocument::box('true'));
        $this->assertSame(false, XmlDocument::box('false'));
        $this->assertSame('foo', XmlDocument::box('foo'));
        $this->assertSame(null, XmlDocument::box('null'));
    }

    public function testUnbox() {
        $this->assertSame('123.45', XmlDocument::unbox('123.45'));
        $this->assertSame('123.45', XmlDocument::unbox(123.45));
        $this->assertSame('123', XmlDocument::unbox('123'));
        $this->assertSame('true', XmlDocument::unbox(true));
        $this->assertSame('false', XmlDocument::unbox('false'));
        $this->assertSame('foo', XmlDocument::unbox('foo'));
        $this->assertSame('null', XmlDocument::unbox(null));
    }

    public function testFormatName() {
        $this->assertEquals('foo', XmlDocument::formatName('foo'));
        $this->assertEquals('foobar', XmlDocument::formatName('foo bar'));
        $this->assertEquals('ns:foo', XmlDocument::formatName('ns:foo'));
        $this->assertEquals('foo-bar', XmlDocument::formatName('foo-bar'));
        $this->assertEquals('foo.bar', XmlDocument::formatName('foo.bar'));
        $this->assertEquals('_123', XmlDocument::formatName('123'));
        $this->assertEquals('_.foo', XmlDocument::formatName('.foo'));
        $this->assertEquals('_-foo', XmlDocument::formatName('-foo'));
    }

    public function testFromMap() {
        $map = Map {
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

        $xml = <<<XML
<?xml version="1.0" encoding="UTF-8"?>
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

        $this->assertEquals($xml, XmlDocument::fromMap('unit', $map)->toString());
    }

    public function testFromMapWithAttributes() {
        $map = Map {
            'name' => 'Barbarian',
            'life' => Map {'@value' => 50},
            'mana' => Map {'@value' => 100, '@attributes' => Map {'max' => 250}},
            'stamina' => 15,
            'vitality' => 20,
            'dexterity' => Map {'@value' => '', '@attributes' => Map {'evade' => '5%', 'block' => '10%'}},
            'agility' => Map {'@value' => '', '@attributes' => Map {'turnRate' => '1.25', 'acceleration' => 5}},
            'armors' => Map {
                'armor' => Vector {
                    Map {'@value' => 'Helmet', '@attributes' => Map {'defense' => 15}},
                    Map {'@value' => 'Shoulder Plates', '@attributes' => Map {'defense' => 25}},
                    Map {'@value' => 'Breast Plate', '@attributes' => Map {'defense' => 50}},
                    Map {'@value' => 'Greaves', '@attributes' => Map {'defense' => 10}},
                    Map {'@value' => 'Gloves', '@attributes' => Map {'defense' => 10}},
                    Map {'@value' => 'Shield', '@attributes' => Map {'defense' => 25}},
                },
                '@attributes' => Map {'items' => 6}
            },
            'weapons' => Map {
                'sword' => Vector {
                    Map {'@value' => 'Broadsword', '@attributes' => Map {'damage' => 25}},
                    Map {'@value' => 'Longsword', '@attributes' => Map {'damage' => 30}}
                },
                'axe' => Vector {
                    Map {'@value' => 'Heavy Axe', '@attributes' => Map {'damage' => 20}},
                    Map {'@value' => 'Double-edged Axe', '@attributes' => Map {'damage' => 25}},
                },
                'polearm' => Map {'@value' => 'Polearm', '@attributes' => Map {'damage' => 50, 'range' => 3, 'speed' => 'slow'}},
                'mace' => Map {'@value' => 'Mace', '@attributes' => Map {'damage' => 15, 'speed' => 'fast'}},
                '@attributes' => Map {'items' => 6}
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
                'scrap' => Map {'@value' => 'Scrap', '@attributes' => Map {'count' => 25}}
            }
        };

        $xml = <<<XML
<?xml version="1.0" encoding="UTF-8"?>
<unit>
    <name>Barbarian</name>
    <life>50</life>
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

        $this->assertEquals($xml, XmlDocument::fromMap('unit', $map)->toString());
    }

    public function testFromMapWithCdata() {
        $map = Map {
            'data' => Map {
                '@value' => 'foobar',
                '@cdata' => true,
                '@attributes' => Map {
                    'baz' => 'qux'
                }
            }
        };

        $xml = <<<XML
<?xml version="1.0" encoding="UTF-8"?>
<root>
    <data baz="qux"><![CDATA[
foobar
]]></data>
</root>

XML;

        $this->assertEquals($xml, XmlDocument::fromMap('root', $map)->toString());
    }

    public function testFromFile() {
        $xml = XmlDocument::fromFile(TEMP_DIR . '/type/barbarian.xml');

        $expected = new XmlElement('unit');

        $name = (new XmlElement('name'))->setValue('Barbarian');
        $life = (new XmlElement('life', Map {'max' => 150}))->setValue('50');
        $mana = (new XmlElement('mana', Map {'max' => 250}))->setValue('100');
        $stamina = (new XmlElement('stamina'))->setValue('15');
        $vitality = (new XmlElement('vitality'))->setValue('20');
        $dexterity = new XmlElement('dexterity', Map {'evade' => '5%', 'block' => '10%'});
        $agility = new XmlElement('agility', Map {'turnRate' => '1.25', 'acceleration' => '5'});
        $armors = new XmlElement('armors', Map {'items' => 6});

            $armor1 = (new XmlElement('armor', Map {'defense' => 15}))->setValue('Helmet');
            $armor2 = (new XmlElement('armor', Map {'defense' => 25}))->setValue('Shoulder Plates');
            $armor3 = (new XmlElement('armor', Map {'defense' => 50}))->setValue('Breast Plate');
            $armor4 = (new XmlElement('armor', Map {'defense' => 10}))->setValue('Greaves');
            $armor5 = (new XmlElement('armor', Map {'defense' => 10}))->setValue('Gloves');
            $armor6 = (new XmlElement('armor', Map {'defense' => 25}))->setValue('Shield');

            $armors->addChildren(Vector {$armor1, $armor2, $armor3, $armor4, $armor5, $armor6});

        $weapons = new XmlElement('weapons', Map {'items' => 6});

            $sword1 = (new XmlElement('sword', Map {'damage' => 25}))->setValue('Broadsword');
            $sword2 = (new XmlElement('sword', Map {'damage' => 30}))->setValue('Longsword');
            $axe1 = (new XmlElement('axe', Map {'damage' => 20}))->setValue('Heavy Axe');
            $axe2 = (new XmlElement('axe', Map {'damage' => 25}))->setValue('Double-edged Axe');
            $polearm = (new XmlElement('polearm', Map {'damage' => 50, 'range' => 3, 'speed' => 'slow'}))->setValue('Polearm');
            $mace = (new XmlElement('mace', Map {'damage' => 15, 'speed' => 'fast'}))->setValue('Mace');

            $weapons->addChildren(Vector {$sword1, $sword2, $axe1, $axe2, $polearm, $mace});

        $items = new XmlElement('items');

            $potions = new XmlElement('potions');

                $potion1 = (new XmlElement('potion'))->setValue('Health Potion');
                $potion2 = (new XmlElement('potion'))->setValue('Mana Potion');

                $potions->addChildren(Vector {$potion1, $potion2});

            $keys = new XmlElement('keys');

                $key1 = (new XmlElement('chestKey'))->setValue('Chest Key');
                $key2 = (new XmlElement('bossKey'))->setValue('Boss Key');

                $keys->addChildren(Vector {$key1, $key2});

            $food1 = (new XmlElement('food'))->setValue('Fruit');
            $food2 = (new XmlElement('food'))->setValue('Bread');
            $food3 = (new XmlElement('food'))->setValue('Vegetables');
            $scrap = (new XmlElement('scrap', Map {'count' => 25}))->setValue('Scrap');

            $items->addChildren(Vector {$potions, $keys, $food1, $food2, $food3, $scrap});

        $expected->addChildren(Vector {$name, $life, $mana, $stamina, $vitality, $dexterity, $agility, $armors, $weapons, $items});

        $this->assertEquals($expected, $xml);
    }

    public function testFromFileStringComparison() {
        $path = TEMP_DIR . '/type/barbarian.xml';
        $xml = XmlDocument::fromFile($path);

        $this->assertEquals(file_get_contents($path), $xml->toString());
    }

    /**
     * @expectedException \Titon\Type\Exception\MissingFileException
     */
    public function testFromFileMissingFile() {
        XmlDocument::fromFile(TEMP_DIR . '/type/barbarian-missing.xml');
    }

    public function testFromVector() {
        $list = Vector {'Helmet', 'Shoulder Plates', 'Breast Plate', 'Greaves', 'Gloves', 'Shield'};

        $xml = <<<XML
<?xml version="1.0" encoding="UTF-8"?>
<armors>
    <armor>Helmet</armor>
    <armor>Shoulder Plates</armor>
    <armor>Breast Plate</armor>
    <armor>Greaves</armor>
    <armor>Gloves</armor>
    <armor>Shield</armor>
</armors>

XML;

        $this->assertEquals($xml, XmlDocument::fromVector('armors', 'armor', $list)->toString());
    }

    public function testFromVectorWithAttributes() {
        $list = Vector {
            'Helmet',
            'Shoulder Plates',
            'Breast Plate',
            Map {'@value' => 'Greaves', '@attributes' => Map {'defense' => 15}},
            'Gloves',
            'Shield'
        };

        $xml = <<<XML
<?xml version="1.0" encoding="UTF-8"?>
<armors>
    <armor>Helmet</armor>
    <armor>Shoulder Plates</armor>
    <armor>Breast Plate</armor>
    <armor defense="15">Greaves</armor>
    <armor>Gloves</armor>
    <armor>Shield</armor>
</armors>

XML;

        $this->assertEquals($xml, XmlDocument::fromVector('armors', 'armor', $list)->toString());
    }

}