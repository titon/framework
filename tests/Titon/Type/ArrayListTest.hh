<?hh // strict
namespace Titon\Type;

use Titon\Test\TestCase;

/**
 * @property \Titon\Type\ArrayList $object
 */
class ArrayListTest extends TestCase {

    protected function setUp(): void {
        parent::setUp();

        $this->object = new ArrayList(Vector {'foo', 'bar', 'baz'});
    }

    public function testIterator(): void {
        $array = [];

        foreach ($this->object as $key => $value) {
            $array[$key * 2] = $value;
        }

        $this->assertEquals([0 => 'foo', 2 => 'bar', 4 => 'baz'], $array);
    }

    public function testIndexishConstructors(): void {
        $this->assertEquals(new ArrayList(Vector {'foo'}), new ArrayList(['foo']));
        $this->assertEquals(new ArrayList(Vector {'foo'}), new ArrayList(Vector {'foo'}));
        $this->assertEquals(new ArrayList(Vector {'foo'}), new ArrayList(Map {0 => 'foo'}));
    }

    public function testChainableMethods(): void {
        $this->assertEquals(new ArrayList(Vector {'foo', 'bar', 'baz'}), $this->object);

        $this->object
            ->add('oof')
            ->addAll(['rab', 'zab'])
            ->set(2, 'zzz');

        $this->assertEquals(new ArrayList(Vector {'foo', 'bar', 'zzz', 'oof', 'rab', 'zab'}), $this->object);
    }

    public function testImmutableMethods(): void {
        $this->assertEquals(new ArrayList(Vector {'foo', 'bar', 'baz'}), $this->object);

        $mapped = $this->object->map(fun('strtoupper'));
        $reversed = $mapped->reverse();

        $this->assertEquals(new ArrayList(Vector {'foo', 'bar', 'baz'}), $this->object);
        $this->assertEquals(new ArrayList(Vector {'FOO', 'BAR', 'BAZ'}), $mapped);
        $this->assertEquals(new ArrayList(Vector {'BAZ', 'BAR', 'FOO'}), $reversed);
        $this->assertNotSame($reversed, $this->object);
    }

    public function testAdd(): void {
        $this->assertEquals(new ArrayList(Vector {'foo', 'bar', 'baz'}), $this->object);

        $this->object->add('fop');
        $this->object->add('oof');

        $this->assertEquals(new ArrayList(Vector {'foo', 'bar', 'baz', 'fop', 'oof'}), $this->object);
    }

    public function testAddAll(): void {
        $this->assertEquals(new ArrayList(Vector {'foo', 'bar', 'baz'}), $this->object);

        $this->object->addAll(Vector {'fop', 'oof'});

        $this->assertEquals(new ArrayList(Vector {'foo', 'bar', 'baz', 'fop', 'oof'}), $this->object);
    }

    public function testAt(): void {
        $this->assertEquals('bar', $this->object->at(1));
    }

    /**
     * @expectedException \OutOfBoundsException
     */
    public function testAtThrowsError(): void {
        $this->assertEquals('bar', $this->object->at(5));
    }

    public function testAppend(): void {
        $this->assertEquals(new ArrayList(Vector {'foo', 'bar', 'baz'}), $this->object);

        $list = $this->object->append('fop');
        $list = $list->append('oof');

        $this->assertEquals(new ArrayList(Vector {'foo', 'bar', 'baz'}), $this->object);
        $this->assertEquals(new ArrayList(Vector {'foo', 'bar', 'baz', 'fop', 'oof'}), $list);
    }

    public function testChunk(): void {
        $this->assertEquals(new ArrayList(Vector {
            new ArrayList(Vector {'foo'}),
            new ArrayList(Vector {'bar'}),
            new ArrayList(Vector {'baz'})
        }), $this->object->chunk(1));
    }

    public function testClean(): void {
        $list = new ArrayList(Vector {1, 0, '0', false, true, null, 'foo', '', 0.0});

        $this->assertEquals(new ArrayList(Vector {1, 0, '0', true, 'foo', 0.0}), $list->clean());
    }

    public function testClear(): void {
        $this->assertEquals(new ArrayList(), $this->object->flush());
    }

    public function testClone(): void {
        $vector = Vector {'foo'};
        $list = new ArrayList($vector);
        $clone = clone $list;

        $this->assertNotSame($vector, $clone->value());
    }

    public function testConcat(): void {
        $list1 = $this->object->concat(new ArrayList(Vector {1, 2, 3}));

        $this->assertEquals(new ArrayList(Vector {'foo', 'bar', 'baz', 1, 2, 3}), $list1);

        $list2 = $this->object->concat(new ArrayList(Vector {1, 2, 3}), false);

        $this->assertEquals(new ArrayList(Vector {1, 2, 3, 'foo', 'bar', 'baz'}), $list2);

        $this->assertNotSame($list1, $this->object);
    }

    public function testContains(): void {
        $this->assertTrue($this->object->contains('foo'));
        $this->assertFalse($this->object->contains(123));
    }

    public function testCount(): void {
        $this->assertEquals(3, $this->object->count());
        $this->object->add('fop');
        $this->assertEquals(4, $this->object->count());
    }

    public function testDepth(): void {
        $this->assertEquals(1, $this->object->depth());
    }

    public function testEach(): void {
        $this->assertEquals(new ArrayList(Vector {'FOO', 'BAR', 'BAZ'}), $this->object->each(function(int $key, string $value): string {
            return strtoupper($value);
        }));
    }

    public function testErase(): void {
        $this->assertEquals(new ArrayList(Vector {'bar', 'baz'}), $this->object->erase('foo'));
    }

    public function testFilter(): void {
        $this->assertEquals(new ArrayList(Vector {'bar', 'baz'}), $this->object->filter(function(string $value): bool {
            return (strpos($value, 'b') !== false);
        }));
    }

    public function testFilterWithKey(): void {
        $this->assertEquals(new ArrayList(Vector {'baz'}), $this->object->filterWithKey(function(int $index, string $value): bool {
            return (strpos($value, 'b') !== false && $index % 2 === 0);
        }));
    }

    public function testFirst(): void {
        $this->assertEquals('foo', $this->object->first());
        $this->assertEquals(null, (new ArrayList())->first());
    }

    public function testFlush(): void {
        $this->assertEquals(new ArrayList(), $this->object->flush());
    }

    public function testGet(): void {
        $this->assertEquals('bar', $this->object->get(1));
        $this->assertEquals(null, $this->object->get(4));
    }

    public function testHas(): void {
        $this->assertTrue($this->object->has(1));
        $this->assertFalse($this->object->has(5));
    }

    public function testIsEmpty(): void {
        $this->assertFalse($this->object->isEmpty());
        $this->object->clear();
        $this->assertTrue($this->object->isEmpty());
    }

    public function testJsonSerialize(): void {
        $this->assertEquals(['foo', 'bar', 'baz'], $this->object->jsonSerialize());
    }

    public function testKeyOf(): void {
        $this->assertEquals(2, $this->object->keyOf('baz'));
        $this->assertEquals(-1, $this->object->keyOf('fop'));
    }

    public function testKeys(): void {
        $this->assertEquals(Vector {0, 1, 2}, $this->object->keys());
        $this->object->add('fop');
        $this->assertEquals(Vector {0, 1, 2, 3}, $this->object->keys());
    }

    public function testLast(): void {
        $this->assertEquals('baz', $this->object->last());
        $this->assertEquals(null, (new ArrayList())->last());
    }

    public function testLength(): void {
        $this->assertEquals(3, $this->object->length());
        $this->object->add('fop');
        $this->assertEquals(4, $this->object->length());
    }

    public function testMap(): void {
        $this->assertEquals(new ArrayList(Vector {'Foo', 'Bar', 'Baz'}), $this->object->map(function(string $value): string {
            return ucfirst($value);
        }));
    }

    public function testMapWithKey(): void {
        $this->assertEquals(new ArrayList(Vector {'Foo0', 'Bar1', 'Baz2'}), $this->object->mapWithKey(function(int $index, string $value): string {
            return ucfirst($value) . $index;
        }));
    }

    public function testMerge(): void {
        $this->assertEquals(new ArrayList(Vector {1, 2, 'baz'}), $this->object->merge(new ArrayList(Vector {1, 2})));
    }

    public function testPluck(): void {
        $list = new ArrayList(Vector {
            Map {'key' => 1},
            Map {'key' => 2},
            Map {'key' => 3},
        });

        $this->assertEquals(Vector {1, 2, 3}, $list->pluck(function($value, $key): void {
            return $value['key'];
        }));
    }

    public function testPrepend(): void {
        $this->assertEquals(new ArrayList(Vector {'foo', 'bar', 'baz'}), $this->object);

        $list = $this->object->prepend('fop');
        $list = $list->prepend('oof');

        $this->assertEquals(new ArrayList(Vector {'foo', 'bar', 'baz'}), $this->object);
        $this->assertEquals(new ArrayList(Vector {'oof', 'fop', 'foo', 'bar', 'baz'}), $list);
    }

    public function testRemove(): void {
        $this->assertEquals(new ArrayList(Vector {'bar', 'baz'}), $this->object->remove(0));
    }

    public function testResize(): void {
        $this->assertEquals(new ArrayList(Vector {'foo', 'bar', 'baz'}), $this->object);

        $this->object->resize(1, 'aaa');

        $this->assertEquals(new ArrayList(Vector {'foo'}), $this->object);

        $this->object->resize(3, 'zzz');

        $this->assertEquals(new ArrayList(Vector {'foo', 'zzz', 'zzz'}), $this->object);
    }

    public function testReverse(): void {
        $this->assertEquals(new ArrayList(Vector {'foo', 'bar', 'baz'}), $this->object);
        $this->assertEquals(new ArrayList(Vector {'baz', 'bar', 'foo'}), $this->object->reverse());
    }

    public function testSerialize(): void {
        $this->assertEquals('C:20:"Titon\Type\ArrayList":50:{V:9:"HH\Vector":3:{s:3:"foo";s:3:"bar";s:3:"baz";}}', serialize($this->object));
        $this->assertEquals('V:9:"HH\Vector":3:{s:3:"foo";s:3:"bar";s:3:"baz";}', $this->object->serialize());
    }

    public function testSet(): void {
        $this->assertEquals(new ArrayList(Vector {'foo', 'bar', 'baz'}), $this->object);

        $this->object->set(0, 'oof');

        $this->assertEquals(new ArrayList(Vector {'oof', 'bar', 'baz'}), $this->object);
    }

    public function testSetAll(): void {
        $this->assertEquals(new ArrayList(Vector {'foo', 'bar', 'baz'}), $this->object);

        $this->object->setAll(Map {
            0 => 'oof',
            2 => 'zab'
        });

        $this->assertEquals(new ArrayList(Vector {'oof', 'bar', 'zab'}), $this->object);
    }

    public function testShuffle(): void {
        $vector = Vector {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
        $list = new ArrayList($vector);

        $this->assertNotEquals(new ArrayList($vector), $list->shuffle());
    }

    public function testSome(): void {
        $this->assertTrue($this->object->some(function(int $key, mixed $value): void {
            return is_string($value);
        }));

        $this->assertFalse($this->object->some(function(int $key, mixed $value): void {
            return is_numeric($value);
        }));
    }

    public function testSort(): void {
        $list = new ArrayList([5, 3, 4, 2, 1]);

        $list2 = $list->sort();

        $this->assertEquals(new ArrayList([1, 2, 3, 4, 5]), $list2);
    }

    public function testSortWithCallback(): void {
        $list = new ArrayList([5, 3, 4, 2, 1]);

        $callback = function($a, $b) {
            if ($a == $b) {
                return 0;
            } else if ($a > $b) {
                return -1;
            } else {
                return 1;
            }
        };

        $list2 = $list->sort($callback);

        $this->assertEquals(new ArrayList([5, 4, 3, 2, 1]), $list2);
    }

    public function testSortNatural(): void {
        $list = new ArrayList([
            'item 109',
            'apple',
            'item 1',
            'item 5',
            'orange',
            'ITEM 10',
            'ITEM 55',
            'banana'
        ]);

        // case-insensitive
        $sort1 = $list->sortNatural();

        $this->assertEquals(new ArrayList([
            'apple',
            'banana',
            'item 1',
            'item 5',
            'ITEM 10',
            'ITEM 55',
            'item 109',
            'orange'
        ]), $sort1);

        // case-sensitive
        $sort2 = $list->sortNatural(true);

        $this->assertEquals(new ArrayList([
            'ITEM 10',
            'ITEM 55',
            'apple',
            'banana',
            'item 1',
            'item 5',
            'item 109',
            'orange'
        ]), $sort2);
    }

    public function testSplice(): void {
        $list = $this->object->splice(1, 1);

        $this->assertEquals(new ArrayList(Vector {'foo', 'baz'}), $list);
        $this->assertNotEquals($this->object, $list);
    }

    public function testToArray(): void {
        $this->assertEquals(['foo', 'bar', 'baz'], $this->object->toArray());
    }

    public function testToJson(): void {
        $this->assertEquals('["foo","bar","baz"]', json_encode($this->object));
        $this->assertEquals('["foo","bar","baz"]', $this->object->toJson());
    }

    public function testToXml(): void {
        $this->assertEquals(
            '<?xml version="1.0" encoding="UTF-8"?>' . PHP_EOL .
            '<items>' . PHP_EOL .
            '    <item>foo</item>' . PHP_EOL .
            '    <item>bar</item>' . PHP_EOL .
            '    <item>baz</item>' . PHP_EOL .
            '</items>' . PHP_EOL
        , $this->object->toXml());
    }

    public function testUnserialize(): void {
        $serialized = serialize($this->object);

        $this->assertEquals($this->object, unserialize($serialized));
    }

    public function testUnique(): void {
        $list = new ArrayList(Vector {'foo', 'bar', 'baz', 'foo', 'baz', 'fop'});

        $this->assertEquals(new ArrayList(Vector {'foo', 'bar', 'baz', 'fop'}), $list->unique());
    }

    public function testValue(): void {
        $this->assertEquals(Vector {'foo', 'bar', 'baz'}, $this->object->value());
    }

    public function testValues(): void {
        $this->assertEquals(Vector {'foo', 'bar', 'baz'}, $this->object->values());
    }

}
