<?hh
namespace Titon\Type;

use Titon\Test\TestCase;

/**
 * @property \Titon\Type\HashMap $object
 */
class HashMapTest extends TestCase {

    protected function setUp() {
        parent::setUp();

        $this->object = new HashMap(Map {
            'a' => 'foo',
            'b' => 'bar',
            'c' => 'baz'
        });
    }

    public function testIterator() {
        $array = [];

        foreach ($this->object as $key => $value) {
            $array[strtoupper($key)] = $value;
        }

        $this->assertEquals(['A' => 'foo', 'B' => 'bar', 'C' => 'baz'], $array);
    }

    public function testIndexishConstructors() {
        $this->assertEquals(new HashMap(Map {0 => 'foo'}), new HashMap(['foo']));
        $this->assertEquals(new HashMap(Map {0 => 'foo'}), new HashMap(Vector {'foo'}));
        $this->assertEquals(new HashMap(Map {'a' => 'foo'}), new HashMap(['a' => 'foo']));
        $this->assertEquals(new HashMap(Map {'a' => 'foo'}), new HashMap(Map {'a' => 'foo'}));
    }

    public function testChainableMethods() {
        $this->assertEquals(new HashMap(Map {
            'a' => 'foo',
            'b' => 'bar',
            'c' => 'baz'
        }), $this->object);

        $this->object
            ->add(Pair {'d', 'oof'})
            ->addAll([Pair {'e', 'rab'}, Pair {'f', 'zab'}])
            ->set('c', 'zzz');

        $this->assertEquals(new HashMap(Map {
            'a' => 'foo',
            'b' => 'bar',
            'c' => 'zzz',
            'd' => 'oof',
            'e' => 'rab',
            'f' => 'zab'
        }), $this->object);
    }

    public function testImmutableMethods() {
        $this->assertEquals(new HashMap(Map {
            'a' => 'foo',
            'b' => 'bar',
            'c' => 'baz'
        }), $this->object);

        $mapped = $this->object->map(fun('strtoupper'));

        $this->assertEquals(new HashMap(Map {
            'a' => 'foo',
            'b' => 'bar',
            'c' => 'baz'
        }), $this->object);

        $this->assertEquals(new HashMap(Map {
            'a' => 'FOO',
            'b' => 'BAR',
            'c' => 'BAZ'
        }), $mapped);

        $this->assertNotSame($mapped, $this->object);
    }

    public function testAdd() {
        $this->assertEquals(new HashMap(Map {
            'a' => 'foo',
            'b' => 'bar',
            'c' => 'baz'
        }), $this->object);

        $this->object->add(Pair {'d', 'fop'});
        $this->object->add(Pair {'e', 'oof'});

        $this->assertEquals(new HashMap(Map {
            'a' => 'foo',
            'b' => 'bar',
            'c' => 'baz',
            'd' => 'fop',
            'e' => 'oof'
        }), $this->object);
    }

    public function testAddAll() {
        $this->assertEquals(new HashMap(Map {
            'a' => 'foo',
            'b' => 'bar',
            'c' => 'baz'
        }), $this->object);

        $this->object->addAll(Vector {Pair {'d', 'fop'}, Pair {'e', 'oof'}});

        $this->assertEquals(new HashMap(Map {
            'a' => 'foo',
            'b' => 'bar',
            'c' => 'baz',
            'd' => 'fop',
            'e' => 'oof'
        }), $this->object);
    }

    public function testAt() {
        $this->assertEquals('bar', $this->object->at('b'));
    }

    /**
     * @expectedException \OutOfBoundsException
     */
    public function testAtThrowsError() {
        $this->assertEquals('bar', $this->object->at('z'));
    }

    public function testChunk() {
        $this->assertEquals(new ArrayList(Vector {
            new HashMap(Map {'a' => 'foo'}),
            new HashMap(Map {'b' => 'bar'}),
            new HashMap(Map {'c' => 'baz'})
        }), $this->object->chunk(1));
    }

    public function testClean() {
        $list = new HashMap(Vector {1, 0, '0', false, true, null, 'foo', ''});

        $this->assertEquals(new HashMap(Map {
            0 => 1,
            1 => 0,
            2 => '0',
            4 => true,
            6 => 'foo'
        }), $list->clean());
    }

    public function testClear() {
        $this->assertEquals(new HashMap(), $this->object->flush());
    }

    public function testClone() {
        $map = Map {'a' => 'foo'};
        $list = new HashMap($map);
        $clone = clone $list;

        $this->assertNotSame($map, $clone->value());
    }

    public function testConcat() {
        $list1 = $this->object->concat(new HashMap(Map {'x' => 1, 'y' => 2, 'z' => 3}));

        $this->assertEquals(new HashMap(Map {
            'a' => 'foo',
            'b' => 'bar',
            'c' => 'baz',
            'x' => 1,
            'y' => 2,
            'z' => 3
        }), $list1);

        $list2 = $this->object->concat(new HashMap(Map {'x' => 1, 'y' => 2, 'z' => 3}), false);

        $this->assertEquals(new HashMap(Map {
            'x' => 1,
            'y' => 2,
            'z' => 3,
            'a' => 'foo',
            'b' => 'bar',
            'c' => 'baz',
        }), $list2);

        $this->assertNotSame($list1, $this->object);
    }

    public function testContains() {
        $this->assertTrue($this->object->contains('foo'));
        $this->assertFalse($this->object->contains(123));
    }

    public function testCount() {
        $this->assertEquals(3, $this->object->count());
        $this->object->add(Pair {'d', 'fop'});
        $this->assertEquals(4, $this->object->count());
    }

    public function testDepth() {
        $this->assertEquals(1, $this->object->depth());
    }

    public function testEach() {
        $this->assertEquals(new HashMap(Map {'a' => 'FOO', 'b' => 'BAR', 'c' => 'BAZ'}), $this->object->each(function(string $key, string $value): string {
            return strtoupper($value);
        }));
    }

    public function testErase() {
        $this->assertEquals(new HashMap(Map {'b' => 'bar', 'c' => 'baz'}), $this->object->erase('foo'));
    }

    public function testFilter() {
        $this->assertEquals(new HashMap(Map {'b' => 'bar', 'c' => 'baz'}), $this->object->filter(function(string $value): bool {
            return (strpos($value, 'b') !== false);
        }));
    }

    public function testFilterWithKey() {
        $this->assertEquals(new HashMap(Map {'c' => 'baz'}), $this->object->filterWithKey(function(string $index, string $value): bool {
            return ($index === 'c');
        }));
    }

    public function testFirst() {
        $this->assertEquals('foo', $this->object->first());
        $this->assertEquals(null, (new HashMap())->first());
    }

    public function testFlush() {
        $this->assertEquals(new HashMap(), $this->object->flush());
    }

    public function testGet() {
        $this->assertEquals('bar', $this->object->get('b'));
        $this->assertEquals(null, $this->object->get('z'));
    }

    public function testHas() {
        $this->assertTrue($this->object->has('b'));
        $this->assertFalse($this->object->has('z'));
    }

    public function testIsEmpty() {
        $this->assertFalse($this->object->isEmpty());
        $this->object->clear();
        $this->assertTrue($this->object->isEmpty());
    }

    public function testJsonSerialize() {
        $this->assertEquals(['a' => 'foo', 'b' => 'bar', 'c' => 'baz'], $this->object->jsonSerialize());
    }

    public function testKeyOf() {
        $this->assertEquals('c', $this->object->keyOf('baz'));
        $this->assertEquals(null, $this->object->keyOf('fop'));
    }

    public function testKeys() {
        $this->assertEquals(Vector {'a', 'b', 'c'}, $this->object->keys());
        $this->object->set('d', 'fop');
        $this->assertEquals(Vector {'a', 'b', 'c', 'd'}, $this->object->keys());
    }

    public function testLast() {
        $this->assertEquals('baz', $this->object->last());
        $this->assertEquals(null, (new HashMap())->last());
    }

    public function testLength() {
        $this->assertEquals(3, $this->object->length());
        $this->object->set('d', 'fop');
        $this->assertEquals(4, $this->object->length());
    }

    public function testMap() {
        $this->assertEquals(new HashMap(Map {'a' => 'Foo', 'b' => 'Bar', 'c' => 'Baz'}), $this->object->map(function(string $value): string {
            return ucfirst($value);
        }));
    }

    public function testMapWithKey() {
        $this->assertEquals(new HashMap(Map {'a' => 'FooA', 'b' => 'BarB', 'c' => 'BazC'}), $this->object->mapWithKey(function(string $index, string $value): string {
            return ucfirst($value) . strtoupper($index);
        }));
    }

    public function testMerge() {
        $this->assertEquals(new HashMap(Map {'a' => 1, 'b' => 2, 'c' => 'baz'}), $this->object->merge(new HashMap(Map {'a' => 1, 'b' => 2})));
    }

    public function testRemove() {
        $this->assertEquals(new HashMap(Map {'b' => 'bar', 'c' => 'baz'}), $this->object->remove('a'));
    }

    public function testReverse() {
        $this->assertEquals(new HashMap(Map {'a' => 'foo', 'b' => 'bar', 'c' => 'baz'}), $this->object);
        $this->assertEquals(new HashMap(Map {'c' => 'baz', 'b' => 'bar', 'a' => 'foo'}), $this->object->reverse());
    }

    public function testSerialize() {
        $this->assertEquals('C:18:"Titon\Type\HashMap":71:{K:6:"HH\Map":3:{s:1:"a";s:3:"foo";s:1:"b";s:3:"bar";s:1:"c";s:3:"baz";}}', serialize($this->object));
        $this->assertEquals('K:6:"HH\Map":3:{s:1:"a";s:3:"foo";s:1:"b";s:3:"bar";s:1:"c";s:3:"baz";}', $this->object->serialize());
    }

    public function testSet() {
        $this->assertEquals(new HashMap(Map {
            'a' => 'foo',
            'b' => 'bar',
            'c' => 'baz'
        }), $this->object);

        $this->object->set('a', 'oof');

        $this->assertEquals(new HashMap(Map {
            'a' => 'oof',
            'b' => 'bar',
            'c' => 'baz'
        }), $this->object);
    }

    public function testSetAll() {
        $this->assertEquals(new HashMap(Map {
            'a' => 'foo',
            'b' => 'bar',
            'c' => 'baz'
        }), $this->object);

        $this->object->setAll(Map {
            'a' => 'oof',
            'c' => 'zab'
        });

        $this->assertEquals(new HashMap(Map {
            'a' => 'oof',
            'b' => 'bar',
            'c' => 'zab'
        }), $this->object);
    }

    public function testShuffle() {
        $vector = Vector {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
        $list = new HashMap($vector);

        $this->assertNotEquals(new HashMap($vector), $list->shuffle());
    }

    public function testSome() {
        $this->assertTrue($this->object->some(function(string $key, mixed $value) {
            return is_string($value);
        }));

        $this->assertFalse($this->object->some(function(string $key, mixed $value) {
            return is_numeric($value);
        }));
    }

    public function testToArray() {
        $this->assertEquals(['a' => 'foo', 'b' => 'bar', 'c' => 'baz'], $this->object->toArray());
    }

    public function testToJson() {
        $this->assertEquals('{"a":"foo","b":"bar","c":"baz"}', json_encode($this->object));
        $this->assertEquals('{"a":"foo","b":"bar","c":"baz"}', $this->object->toJson());
    }

    public function testToXml() {
        $this->markTestSkipped('Need to implement!');
    }

    public function testUnserialize() {
        $serialized = serialize($this->object);

        $this->assertEquals($this->object, unserialize($serialized));
    }

    public function testUnique() {
        $list = new HashMap(Vector {'foo', 'bar', 'baz', 'foo', 'baz', 'fop'});

        $this->assertEquals(new HashMap(Map {
            0 => 'foo',
            1 => 'bar',
            2 => 'baz',
            5 => 'fop'
        }), $list->unique());
    }

    public function testValue() {
        $this->assertEquals(Map {
            'a' => 'foo',
            'b' => 'bar',
            'c' => 'baz'
        }, $this->object->value());
    }

    public function testValues() {
        $this->assertEquals(Vector {'foo', 'bar', 'baz'}, $this->object->values());
    }

}