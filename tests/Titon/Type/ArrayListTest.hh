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

    public function testIterator() {
        $array = [];

        foreach ($this->object as $key => $value) {
            $array[$key * 2] = $value;
        }

        $this->assertEquals([0 => 'foo', 2 => 'bar', 4 => 'baz'], $array);
    }

    public function testIndexishConstructors() {
        $this->assertEquals(new ArrayList(Vector {'foo'}), new ArrayList(['foo']));
        $this->assertEquals(new ArrayList(Vector {'foo'}), new ArrayList(Vector {'foo'}));
        $this->assertEquals(new ArrayList(Vector {'foo'}), new ArrayList(Map {0 => 'foo'}));
    }

    public function testChainableMethods() {
        $this->assertEquals(new ArrayList(Vector {'foo', 'bar', 'baz'}), $this->object);

        $this->object
            ->add('oof')
            ->addAll(['rab', 'zab'])
            ->set(2, 'zzz');

        $this->assertEquals(new ArrayList(Vector {'foo', 'bar', 'zzz', 'oof', 'rab', 'zab'}), $this->object);
    }

    public function testImmutableMethods() {
        $this->assertEquals(new ArrayList(Vector {'foo', 'bar', 'baz'}), $this->object);

        $mapped = $this->object->map(fun('strtoupper'));
        $reversed = $mapped->reverse();

        $this->assertEquals(new ArrayList(Vector {'foo', 'bar', 'baz'}), $this->object);
        $this->assertEquals(new ArrayList(Vector {'FOO', 'BAR', 'BAZ'}), $mapped);
        $this->assertEquals(new ArrayList(Vector {'BAZ', 'BAR', 'FOO'}), $reversed);
        $this->assertNotSame($reversed, $this->object);
    }

    public function testAdd() {
        $this->assertEquals(new ArrayList(Vector {'foo', 'bar', 'baz'}), $this->object);

        $this->object->add('fop');
        $this->object->add('oof');

        $this->assertEquals(new ArrayList(Vector {'foo', 'bar', 'baz', 'fop', 'oof'}), $this->object);
    }

    public function testAddAll() {
        $this->assertEquals(new ArrayList(Vector {'foo', 'bar', 'baz'}), $this->object);

        $this->object->addAll(Vector {'fop', 'oof'});

        $this->assertEquals(new ArrayList(Vector {'foo', 'bar', 'baz', 'fop', 'oof'}), $this->object);
    }

    public function testAt() {
        $this->assertEquals('bar', $this->object->at(1));
    }

    /**
     * @expectedException \OutOfBoundsException
     */
    public function testAtThrowsError() {
        $this->assertEquals('bar', $this->object->at(5));
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

    public function testClear() {
        $this->assertEquals(new ArrayList(), $this->object->flush());
    }

    public function testClone() {
        $vector = Vector {'foo'};
        $list = new ArrayList($vector);
        $clone = clone $list;

        $this->assertNotSame($vector, $clone->value());
    }

    public function testConcat() {
        $list1 = $this->object->concat(new ArrayList(Vector {1, 2, 3}));

        $this->assertEquals(new ArrayList(Vector {'foo', 'bar', 'baz', 1, 2, 3}), $list1);

        $list2 = $this->object->concat(new ArrayList(Vector {1, 2, 3}), false);

        $this->assertEquals(new ArrayList(Vector {1, 2, 3, 'foo', 'bar', 'baz'}), $list2);

        $this->assertNotSame($list1, $this->object);
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

    public function testFilter() {
        $this->assertEquals(new ArrayList(Vector {'bar', 'baz'}), $this->object->filter(function(string $value): bool {
            return (strpos($value, 'b') !== false);
        }));
    }

    public function testFilterWithKey() {
        $this->assertEquals(new ArrayList(Vector {'baz'}), $this->object->filterWithKey(function(int $index, string $value): bool {
            return (strpos($value, 'b') !== false && $index % 2 === 0);
        }));
    }

    public function testFirst() {
        $this->assertEquals('foo', $this->object->first());
        $this->assertEquals(null, (new ArrayList())->first());
    }

    public function testFlush() {
        $this->assertEquals(new ArrayList(), $this->object->flush());
    }

    public function testGet() {
        $this->assertEquals('bar', $this->object->get(1));
        $this->assertEquals(null, $this->object->get(4));
    }

    public function testHas() {
        $this->assertTrue($this->object->has(1));
        $this->assertFalse($this->object->has(5));
    }

    public function testIsEmpty() {
        $this->assertFalse($this->object->isEmpty());
        $this->object->clear();
        $this->assertTrue($this->object->isEmpty());
    }

    public function testJsonSerialize() {
        $this->assertEquals(['foo', 'bar', 'baz'], $this->object->jsonSerialize());
    }

    public function testKeyOf() {
        $this->assertEquals(2, $this->object->keyOf('baz'));
        $this->assertEquals(-1, $this->object->keyOf('fop'));
    }

    public function testKeys() {
        $this->assertEquals(Vector {0, 1, 2}, $this->object->keys());
        $this->object->add('fop');
        $this->assertEquals(Vector {0, 1, 2, 3}, $this->object->keys());
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

    public function testMap() {
        $this->assertEquals(new ArrayList(Vector {'Foo', 'Bar', 'Baz'}), $this->object->map(function(string $value): string {
            return ucfirst($value);
        }));
    }

    public function testMapWithKey() {
        $this->assertEquals(new ArrayList(Vector {'Foo0', 'Bar1', 'Baz2'}), $this->object->mapWithKey(function(int $index, string $value): string {
            return ucfirst($value) . $index;
        }));
    }

    public function testMerge() {
        $this->assertEquals(new ArrayList(Vector {1, 2, 'baz'}), $this->object->merge(new ArrayList(Vector {1, 2})));
    }

    public function testRemove() {
        $this->assertEquals(new ArrayList(Vector {'bar', 'baz'}), $this->object->remove(0));
    }

    public function testResize() {
        $this->assertEquals(new ArrayList(Vector {'foo', 'bar', 'baz'}), $this->object);

        $this->object->resize(1, 'aaa');

        $this->assertEquals(new ArrayList(Vector {'foo'}), $this->object);

        $this->object->resize(3, 'zzz');

        $this->assertEquals(new ArrayList(Vector {'foo', 'zzz', 'zzz'}), $this->object);
    }

    public function testReverse() {
        $this->assertEquals(new ArrayList(Vector {'foo', 'bar', 'baz'}), $this->object);
        $this->assertEquals(new ArrayList(Vector {'baz', 'bar', 'foo'}), $this->object->reverse());
    }

    public function testSerialize() {
        $this->assertEquals('C:20:"Titon\Type\ArrayList":50:{V:9:"HH\Vector":3:{s:3:"foo";s:3:"bar";s:3:"baz";}}', serialize($this->object));
        $this->assertEquals('V:9:"HH\Vector":3:{s:3:"foo";s:3:"bar";s:3:"baz";}', $this->object->serialize());
    }

    public function testSet() {
        $this->assertEquals(new ArrayList(Vector {'foo', 'bar', 'baz'}), $this->object);

        $this->object->set(0, 'oof');

        $this->assertEquals(new ArrayList(Vector {'oof', 'bar', 'baz'}), $this->object);
    }

    public function testSetAll() {
        $this->assertEquals(new ArrayList(Vector {'foo', 'bar', 'baz'}), $this->object);

        $this->object->setAll(Map {
            0 => 'oof',
            2 => 'zab'
        });

        $this->assertEquals(new ArrayList(Vector {'oof', 'bar', 'zab'}), $this->object);
    }

    public function testShuffle() {
        $vector = Vector {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
        $list = new ArrayList($vector);

        $this->assertNotEquals(new ArrayList($vector), $list->shuffle());
    }

    public function testSome() {
        $this->assertTrue($this->object->some(function(int $key, mixed $value) {
            return is_string($value);
        }));

        $this->assertFalse($this->object->some(function(int $key, mixed $value) {
            return is_numeric($value);
        }));
    }

    public function testSplice() {
        $list = $this->object->splice(1, 1);

        $this->assertEquals(new ArrayList(Vector {'foo', 'baz'}), $list);
        $this->assertNotEquals($this->object, $list);
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

    public function testUnique() {
        $list = new ArrayList(Vector {'foo', 'bar', 'baz', 'foo', 'baz', 'fop'});

        $this->assertEquals(new ArrayList(Vector {'foo', 'bar', 'baz', 'fop'}), $list->unique());
    }

    public function testValue() {
        $this->assertEquals(Vector {'foo', 'bar', 'baz'}, $this->object->value());
    }

    public function testValues() {
        $this->assertEquals(Vector {'foo', 'bar', 'baz'}, $this->object->values());
    }

}