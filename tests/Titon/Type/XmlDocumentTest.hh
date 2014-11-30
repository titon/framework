<?hh
namespace Titon\Type;

use Titon\Test\TestCase;

class XmlDocumentTest extends TestCase {

    public function testAutobox() {
        $this->assertSame(123.45, XmlDocument::autobox('123.45'));
        $this->assertSame(123.45, XmlDocument::autobox('123.45'));
        $this->assertSame(123, XmlDocument::autobox('123'));
        $this->assertSame(true, XmlDocument::autobox('true'));
        $this->assertSame(false, XmlDocument::autobox('false'));
        $this->assertSame('foo', XmlDocument::autobox('foo'));
    }

    public function testUnbox() {
        $this->assertSame('123.45', XmlDocument::unbox('123.45'));
        $this->assertSame('123.45', XmlDocument::unbox(123.45));
        $this->assertSame('123', XmlDocument::unbox('123'));
        $this->assertSame('true', XmlDocument::unbox(true));
        $this->assertSame('false', XmlDocument::unbox('false'));
        $this->assertSame('foo', XmlDocument::unbox('foo'));
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

        $xml = <<<XML
<?xml version="1.0" encoding="UTF-8"?>
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

        $this->assertEquals($xml, XmlDocument::fromMap('unit', $map)->toString());
    }

    public function testFromMapWithCdata() {
        $map = Map {
            'data' => Map {'value' => 'foobar', 'cdata' => true, 'baz' => 'qux'}
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

}