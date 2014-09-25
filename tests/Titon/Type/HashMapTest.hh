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

    public function testIndexOf() {
        $this->assertEquals(0, $this->object->indexOf('a'));
        $this->assertEquals(2, $this->object->indexOf('c'));
        $this->assertEquals(-1, $this->object->indexOf('z'));
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

    public function testSort() {
        $map = new HashMap([
            'a' => 5,
            'b' => 3,
            'c' => 4,
            'd' => 1,
            'e' => 2
        ]);

        $map2 = $map->sort();

        $this->assertEquals(new HashMap([
            'd' => 1,
            'e' => 2,
            'b' => 3,
            'c' => 4,
            'a' => 5
        ]), $map2);
    }

    public function testSortWithCallback() {
        $map = new HashMap([
            'a' => 5,
            'b' => 3,
            'c' => 4,
            'd' => 1,
            'e' => 2
        ]);

        $callback = function($a, $b) {
            if ($a == $b) {
                return 0;
            } else if ($a > $b) {
                return -1;
            } else {
                return 1;
            }
        };

        $map2 = $map->sort($callback);

        $this->assertEquals(new HashMap([
            'a' => 5,
            'c' => 4,
            'b' => 3,
            'e' => 2,
            'd' => 1,
        ]), $map2);
    }

    public function testSortBy() {
        $books = new HashMap([
            ['id' => 1, 'series_id' => 1, 'name' => 'A Game of Thrones', 'isbn' => '0-553-10354-7', 'released' => '1996-08-02'],
            ['id' => 2, 'series_id' => 1, 'name' => 'A Clash of Kings', 'isbn' => '0-553-10803-4', 'released' => '1999-02-25'],
            ['id' => 3, 'series_id' => 1, 'name' => 'A Storm of Swords', 'isbn' => '0-553-10663-5', 'released' => '2000-11-11'],
            ['id' => 4, 'series_id' => 1, 'name' => 'A Feast for Crows', 'isbn' => '0-553-80150-3', 'released' => '2005-11-02'],
            ['id' => 5, 'series_id' => 1, 'name' => 'A Dance with Dragons', 'isbn' => '0-553-80147-3', 'released' => '2011-07-19'],
            ['id' => 6, 'series_id' => 2, 'name' => 'Harry Potter and the Philosopher\'s Stone', 'isbn' => '0-7475-3269-9', 'released' => '1997-06-27'],
            ['id' => 7, 'series_id' => 2, 'name' => 'Harry Potter and the Chamber of Secrets', 'isbn' => '0-7475-3849-2', 'released' => '1998-07-02'],
            ['id' => 8, 'series_id' => 2, 'name' => 'Harry Potter and the Prisoner of Azkaban', 'isbn' => '0-7475-4215-5', 'released' => '1999-07-09'],
            ['id' => 9, 'series_id' => 2, 'name' => 'Harry Potter and the Goblet of Fire', 'isbn' => '0-7475-4624-X', 'released' => '2000-07-08'],
            ['id' => 10, 'series_id' => 2, 'name' => 'Harry Potter and the Order of the Phoenix', 'isbn' => '0-7475-5100-6', 'released' => '2003-06-21'],
            ['id' => 11, 'series_id' => 2, 'name' => 'Harry Potter and the Half-blood Prince', 'isbn' => '0-7475-8108-8', 'released' => '2005-07-16'],
            ['id' => 12, 'series_id' => 2, 'name' => 'Harry Potter and the Deathly Hallows', 'isbn' => '0-545-01022-5', 'released' => '2007-07-21'],
            ['id' => 13, 'series_id' => 3, 'name' => 'The Fellowship of the Ring', 'isbn' => '', 'released' => '1954-07-24'],
            ['id' => 14, 'series_id' => 3, 'name' => 'The Two Towers', 'isbn' => '', 'released' => '1954-11-11'],
            ['id' => 15, 'series_id' => 3, 'name' => 'The Return of the King', 'isbn' => '', 'released' => '1955-10-25'],
        ]);

        $this->assertEquals(new HashMap([
            1 => ['id' => 2, 'series_id' => 1, 'name' => 'A Clash of Kings', 'isbn' => '0-553-10803-4', 'released' => '1999-02-25'],
            4 => ['id' => 5, 'series_id' => 1, 'name' => 'A Dance with Dragons', 'isbn' => '0-553-80147-3', 'released' => '2011-07-19'],
            3 => ['id' => 4, 'series_id' => 1, 'name' => 'A Feast for Crows', 'isbn' => '0-553-80150-3', 'released' => '2005-11-02'],
            0 => ['id' => 1, 'series_id' => 1, 'name' => 'A Game of Thrones', 'isbn' => '0-553-10354-7', 'released' => '1996-08-02'],
            2 => ['id' => 3, 'series_id' => 1, 'name' => 'A Storm of Swords', 'isbn' => '0-553-10663-5', 'released' => '2000-11-11'],
            6 => ['id' => 7, 'series_id' => 2, 'name' => 'Harry Potter and the Chamber of Secrets', 'isbn' => '0-7475-3849-2', 'released' => '1998-07-02'],
            11 => ['id' => 12, 'series_id' => 2, 'name' => 'Harry Potter and the Deathly Hallows', 'isbn' => '0-545-01022-5', 'released' => '2007-07-21'],
            8 => ['id' => 9, 'series_id' => 2, 'name' => 'Harry Potter and the Goblet of Fire', 'isbn' => '0-7475-4624-X', 'released' => '2000-07-08'],
            10 => ['id' => 11, 'series_id' => 2, 'name' => 'Harry Potter and the Half-blood Prince', 'isbn' => '0-7475-8108-8', 'released' => '2005-07-16'],
            9 => ['id' => 10, 'series_id' => 2, 'name' => 'Harry Potter and the Order of the Phoenix', 'isbn' => '0-7475-5100-6', 'released' => '2003-06-21'],
            5 => ['id' => 6, 'series_id' => 2, 'name' => 'Harry Potter and the Philosopher\'s Stone', 'isbn' => '0-7475-3269-9', 'released' => '1997-06-27'],
            7 => ['id' => 8, 'series_id' => 2, 'name' => 'Harry Potter and the Prisoner of Azkaban', 'isbn' => '0-7475-4215-5', 'released' => '1999-07-09'],
            12 => ['id' => 13, 'series_id' => 3, 'name' => 'The Fellowship of the Ring', 'isbn' => '', 'released' => '1954-07-24'],
            14 => ['id' => 15, 'series_id' => 3, 'name' => 'The Return of the King', 'isbn' => '', 'released' => '1955-10-25'],
            13 => ['id' => 14, 'series_id' => 3, 'name' => 'The Two Towers', 'isbn' => '', 'released' => '1954-11-11'],
        ]), $books->sortBy('name'));
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