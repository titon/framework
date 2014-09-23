<?hh
namespace Titon\Type;

use Titon\Test\TestCase;

/**
 * @property \Titon\Type\ArrayList $object
 */
class ArrayListTest extends TestCase {

    protected function setUp() {
        parent::setUp();

        $this->object = new ArrayList(Vector {'foo', 'bar', 'baz'});
    }

    public function testArrayAccess() {
        $this->assertEquals('foo', $this->object[0]);
        $this->assertEquals(null, $this->object[3]);

        $this->assertTrue(isset($this->object[0]));
        $this->assertFalse(isset($this->object[3]));

        $this->object[] = 'fop';
        $this->object[0] = 'oof';

        $this->assertEquals('oof', $this->object[0]);
        $this->assertTrue(isset($this->object[3]));
    }

    public function testIterator() {
        $array = [];

        foreach ($this->object as $key => $value) {
            $array[$key * 2] = $value;
        }

        $this->assertEquals([0 => 'foo', 2 => 'bar', 4 => 'baz'], $array);
    }

    public function testChunk() {
        $this->assertEquals(new ArrayList(Vector {
            new ArrayList(Vector {'foo'}),
            new ArrayList(Vector {'bar'}),
            new ArrayList(Vector {'baz'})
        }), $this->object->chunk(1));
    }

    public function testClean() {
        $list = new ArrayList(Vector {1, 0, '0', false, true, null, 'foo', ''});

        $this->assertEquals(new ArrayList(Vector {1, 0, '0', true, 'foo'}), $list->clean());
    }

    public function testClone() {
        $vector = Vector {'foo'};
        $list = new ArrayList($vector);
        $clone = clone $list;

        $this->assertNotSame($vector, $clone->value());
    }

    public function testConcat() {
        $this->assertEquals(new ArrayList(Vector {'foo', 'bar', 'baz', 1, 2, 3}), $this->object->concat(new ArrayList(Vector {1, 2, 3})));
    }

    public function testContains() {
        $this->assertTrue($this->object->contains('foo'));
        $this->assertFalse($this->object->contains(123));
    }

    public function testCount() {
        $this->assertEquals(3, $this->object->count());
        $this->object->add('fop');
        $this->assertEquals(4, $this->object->count());
    }

    public function testDepth() {
        $this->assertEquals(1, $this->object->depth());
    }

    public function testEach() {
        $this->assertEquals(new ArrayList(Vector {'FOO', 'BAR', 'BAZ'}), $this->object->each(function(int $key, string $value): string {
            return strtoupper($value);
        }));
    }

    public function testErase() {
        $this->assertEquals(new ArrayList(Vector {'bar', 'baz'}), $this->object->erase('foo'));
    }

    public function testFirst() {
        $this->assertEquals('foo', $this->object->first());
        $this->assertEquals(null, (new ArrayList())->first());
    }

    public function testFlush() {
        $this->assertEquals(new ArrayList(), $this->object->flush());
    }

    public function testHas() {
        $this->assertTrue($this->object->has(1));
        $this->assertFalse($this->object->has(5));
    }

    public function testJsonSerialize() {
        $this->assertEquals(['foo', 'bar', 'baz'], $this->object->jsonSerialize());
    }

    public function testKeyOf() {
        $this->assertEquals(2, $this->object->keyOf('baz'));
        $this->assertEquals(-1, $this->object->keyOf('fop'));
    }

    public function testLast() {
        $this->assertEquals('baz', $this->object->last());
        $this->assertEquals(null, (new ArrayList())->last());
    }

    public function testLength() {
        $this->assertEquals(3, $this->object->length());
        $this->object->add('fop');
        $this->assertEquals(4, $this->object->length());
    }

    public function testMerge() {
        $this->assertEquals(new ArrayList(Vector {'foo', 'bar', 'baz', 1, 2, 3}), $this->object->merge(new ArrayList(Vector {1, 2, 3})));
    }

    public function testRemove() {
        $this->assertEquals(new ArrayList(Vector {'bar', 'baz'}), $this->object->remove(0));
    }

    public function testSerialize() {
        $this->assertEquals('C:20:"Titon\Type\ArrayList":50:{V:9:"HH\Vector":3:{s:3:"foo";s:3:"bar";s:3:"baz";}}', serialize($this->object));
        $this->assertEquals('V:9:"HH\Vector":3:{s:3:"foo";s:3:"bar";s:3:"baz";}', $this->object->serialize());
    }

    public function testSome() {
        $this->assertTrue($this->object->some(function(int $key, mixed $value) {
            return is_string($value);
        }));

        $this->assertFalse($this->object->some(function(int $key, mixed $value) {
            return is_numeric($value);
        }));
    }

    public function testToArray() {
        $this->assertEquals(['foo', 'bar', 'baz'], $this->object->toArray());
    }

    public function testToJson() {
        $this->assertEquals('["foo","bar","baz"]', json_encode($this->object));
        $this->assertEquals('["foo","bar","baz"]', $this->object->toJson());
    }

    public function testToXml() {
        $this->markTestSkipped('Need to implement!');
    }

    public function testUnserialize() {
        $serialized = serialize($this->object);

        $this->assertEquals($this->object, unserialize($serialized));
    }

    public function testValue() {
        $this->assertEquals(Vector {'foo', 'bar', 'baz'}, $this->object->value());
    }

    public function testUnique() {
        $list = new ArrayList(Vector {'foo', 'bar', 'baz', 'foo', 'baz', 'fop'});

        $this->assertEquals(new ArrayList(Vector {'foo', 'bar', 'baz', 'fop'}), $list->unique());
    }

}