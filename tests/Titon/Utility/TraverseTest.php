<?hh
namespace Titon\Utility;

use Titon\Test\TestCase;
use \Exception;
use \stdClass;

class TraverseTest extends TestCase {

    protected Map<string, mixed> $map = Map {
        'integer' => 123,
        'number' => '456',
        'string' => 'foobar',
        'boolean' => true,
        'vector' => Vector {},
        'one' => Map {
            'depth' => 1,
            'two' => Map {
                'depth' => 2,
                'three' => Map {
                    'depth' => 3
                }
            }
        }
    };

    protected array $array = [
        'integer' => 123,
        'number' => '456',
        'string' => 'foobar',
        'boolean' => true,
        'vector' => [],
        'one' => [
            'depth' => 1,
            'two' => [
                'depth' => 2,
                'three' => [
                    'depth' => 3
                ]
            ]
        ]
    ];

    protected Map<string, mixed> $collapsed = Map {
        'integer' => 123,
        'number' => '456',
        'string' => 'foobar',
        'boolean' => true,
        'vector' => null,
        'one.depth' => 1,
        'one.two.depth' => 2,
        'one.two.three.depth' => 3
    };

    public function testCheck() {
        $this->assertTrue(Traverse::check([]));
        $this->assertTrue(Traverse::check(Map {}));
        $this->assertTrue(Traverse::check(Set {}));
        $this->assertTrue(Traverse::check(Pair {1, 2}));
        $this->assertTrue(Traverse::check(Vector {}));
    }

    /**
     * @expectedException \InvalidArgumentException
     */
    public function testCheckThrowsException() {
        $this->assertTrue(Traverse::check(123));
    }

    public function testDepth() {
        $map = Map {'one' => Map {'two' => Map {'three' => Map {'four' => Map {'five' => 5}}}}};
        $vector = Vector {Vector {Vector {3}}};
        $set = Set {1};
        // Doesn't work with Pair

        $this->assertEquals(0, Traverse::depth(Vector {}));
        $this->assertEquals(4, Traverse::depth($this->array));
        $this->assertEquals(5, Traverse::depth($map));
        $this->assertEquals(3, Traverse::depth($vector));
        $this->assertEquals(1, Traverse::depth($set));
    }

    public function testEach() {
        $callback = function($value, $key) {
            if (is_numeric($value)) {
                return $value * 2;

            } elseif (is_string($value)) {
                return strtoupper($value);
            }

            return $value;
        };

        $array = [
            'integer' => 123,
            'number' => '456',
            'string' => 'foobar',
            'boolean' => true
        ];

        $this->assertEquals([
            'integer' => 246,
            'number' => 912,
            'string' => 'FOOBAR',
            'boolean' => true
        ], Traverse::each($array, $callback));

        $map = Map {
            'integer' => 123,
            'number' => '456',
            'string' => 'foobar',
            'boolean' => true
        };

        $this->assertEquals(Map {
            'integer' => 246,
            'number' => 912,
            'string' => 'FOOBAR',
            'boolean' => true
        }, Traverse::each($map, $callback));

        $vector = Vector {
            123,
            '456',
            'foobar',
            true
        };

        $this->assertEquals(Vector {
            246,
            912,
            'FOOBAR',
            true
        }, Traverse::each($vector, $callback));
    }

    public function testEachNonRecursive() {
        $callback = function($value, $key) {
            if (is_numeric($value)) {
                return $value * 2;

            } elseif (is_string($value)) {
                return strtoupper($value);
            }

            return $value;
        };

        $data = Map {
            'integer' => 123,
            'number' => '456',
            'string' => 'foobar',
            'boolean' => true,
            'vector' => Vector {
                123,
                '456',
                'foobar',
                true
            }
        };

        $this->assertEquals(Map {
            'integer' => 246,
            'number' => 912,
            'string' => 'FOOBAR',
            'boolean' => true,
            'vector' => Vector {
                123,
                456,
                'foobar',
                true
            }
        }, Traverse::each($data, $callback, false));
    }

    public function testEvery() {
        $callback = function($value, $key) {
            return is_int($value);
        };

        $this->assertTrue(Traverse::every(Vector {123, 456}, $callback));
        $this->assertFalse(Traverse::every(Vector {123, '456'}, $callback));

        $this->assertTrue(Traverse::every(Map {'a' => 123, 'b' => 456}, $callback));
        $this->assertFalse(Traverse::every(Map {'a' => 123, 'b' => '456'}, $callback));

        $this->assertTrue(Traverse::every(['a' => 123, 'b' => 456], $callback));
        $this->assertFalse(Traverse::every(['a' => 123, 'b' => '456'], $callback));
    }

    public function testExclude() {
        $this->assertEquals(['foo' => 123], Traverse::exclude(['foo' => 123, 'bar' => 456, 'baz' => 789], Vector {'bar', 'baz'}));
        $this->assertEquals(Map {'foo' => 123}, Traverse::exclude(Map {'foo' => 123, 'bar' => 456, 'baz' => 789}, Vector {'bar', 'baz'}));
    }

    public function testExpand() {
        $map = $this->map;
        $map->set('vector', null);

        $this->assertEquals($map, Traverse::expand($this->collapsed));
    }

    public function testExtract() {
        $map = $this->map;

        $this->assertEquals(null, Traverse::extract($map, ''));
        $this->assertEquals(null, Traverse::extract($map, 'fake.path'));
        $this->assertEquals(2, Traverse::extract($map, 'one.two.depth'));
        $this->assertEquals(null, Traverse::extract($map, 'one.two.three.four.depth'));
        $this->assertEquals(Map {
            'depth' => 2,
            'three' => Map {
                'depth' => 3
            }
        }, Traverse::extract($map, 'one.two'));
    }

    public function testExtractArray() {
        $array = $this->array;

        $this->assertEquals(null, Traverse::extract($array, ''));
        $this->assertEquals(null, Traverse::extract($array, 'fake.path'));
        $this->assertEquals(2, Traverse::extract($array, 'one.two.depth'));
        $this->assertEquals(null, Traverse::extract($array, 'one.two.three.four.depth'));
        $this->assertEquals([
            'depth' => 2,
            'three' => [
                'depth' => 3
            ]
        ], Traverse::extract($array, 'one.two'));
    }

    public function testFilter() {
        $data = Map {
            'true' => true,
            'false' => false,
            'null' => null,
            'zero' => 0,
            'stringZero' => '0',
            'empty' => Vector {},
            'array' => [1, 2, 3],
            'vector' => Vector {},
            'map' => Map {
                'false' => false,
                'null' => null,
                'empty' => Vector {}
            }
        };

        $this->assertEquals(Map {
            'true' => true,
            'zero' => 0,
            'stringZero' => '0',
            'array' => [1, 2, 3]
        }, Traverse::filter($data));
    }

    public function testFilterArray() {
        $data = [
            'true' => true,
            'false' => false,
            'null' => null,
            'zero' => 0,
            'stringZero' => '0',
            'empty' => [],
            'array' => [1, 2, 3],
            'map' => [
                'false' => false,
                'null' => null,
                'empty' => []
            ]
        ];

        $this->assertEquals([
            'true' => true,
            'zero' => 0,
            'stringZero' => '0',
            'array' => [1, 2, 3]
        ], Traverse::filter($data));
    }

    public function testFilterNonRecursive() {
        $data = Map {
            'true' => true,
            'false' => false,
            'null' => null,
            'zero' => 0,
            'stringZero' => '0',
            'empty' => Vector {},
            'array' => [1, 2, 3],
            'vector' => Vector {},
            'map' => Map {
                'false' => false,
                'null' => null,
                'empty' => Vector {}
            }
        };

        $this->assertEquals(Map {
            'true' => true,
            'zero' => 0,
            'stringZero' => '0',
            'array' => [1, 2, 3],
            'map' => Map {
                'false' => false,
                'null' => null,
                'empty' => Vector {}
            }
        }, Traverse::filter($data, false));
    }

    public function testFilterCallback() {
        $data = Map {
            'true' => true,
            'false' => false,
            'null' => null,
            'zero' => 0,
            'stringZero' => '0',
            'empty' => Vector {},
            'array' => [1, 2, 3],
            'vector' => Vector {},
            'map' => Map {
                'false' => false,
                'null' => null,
                'empty' => Vector {}
            }
        };

        $this->assertEquals(Map {
            'null' => null,
            'zero' => 0,
            'stringZero' => '0',
            'empty' => Vector {},
            'array' => [1, 2, 3],
            'vector' => Vector {},
            'map' => Map {
                'null' => null,
                'empty' => Vector {}
            }
        }, Traverse::filter($data, true, function($value) {
            return !is_bool($value);
        }));
    }

    public function testFlatten() {
        $this->assertEquals($this->collapsed, Traverse::flatten($this->map));
        $this->assertEquals($this->collapsed, Traverse::flatten($this->array));
    }

    public function testFlattenPrefix() {
        $this->assertEquals(Map {
            'prefix.integer' => 123,
            'prefix.number' => '456',
            'prefix.string' => 'foobar',
            'prefix.boolean' => true,
            'prefix.vector' => null,
            'prefix.one.depth' => 1,
            'prefix.one.two.depth' => 2,
            'prefix.one.two.three.depth' => 3
        }, Traverse::flatten($this->map, 'prefix'));
    }

    public function testGet() {
        $map = $this->map;
        $array = $this->array;

        $this->assertEquals($map, Traverse::get($map));
        $this->assertEquals($array, Traverse::get($array));

        $this->assertEquals(Map {
            'depth' => 2,
            'three' => Map {
                'depth' => 3
            }
        }, Traverse::get($map, 'one.two'));

        $this->assertEquals([
            'depth' => 2,
            'three' => [
                'depth' => 3
            ]
        ], Traverse::get($array, 'one.two'));
    }

    public function testHas() {
        $map = $this->map;

        $this->assertTrue(Traverse::has($map, 'boolean'));
        $this->assertTrue(Traverse::has($map, 'vector'));
        $this->assertTrue(Traverse::has($map, 'one.depth'));
        $this->assertTrue(Traverse::has($map, 'one.two.depth'));
        $this->assertTrue(Traverse::has($map, 'one.two.three.depth'));

        $this->assertFalse(Traverse::has(Map {}, 'foo'));
        $this->assertFalse(Traverse::has($map, ''));
        $this->assertFalse(Traverse::has($map, 'one.two.three.some.really.deep.depth'));
        $this->assertFalse(Traverse::has($map, 'foo'));
        $this->assertFalse(Traverse::has($map, 'foo.bar'));
        $this->assertFalse(Traverse::has($map, 'empty.key'));
    }

    public function testHasArray() {
        $array = $this->array;

        $this->assertTrue(Traverse::has($array, 'boolean'));
        $this->assertTrue(Traverse::has($array, 'vector'));
        $this->assertTrue(Traverse::has($array, 'one.depth'));
        $this->assertTrue(Traverse::has($array, 'one.two.depth'));
        $this->assertTrue(Traverse::has($array, 'one.two.three.depth'));

        $this->assertFalse(Traverse::has([], 'foo'));
        $this->assertFalse(Traverse::has($array, ''));
        $this->assertFalse(Traverse::has($array, 'one.two.three.some.really.deep.depth'));
        $this->assertFalse(Traverse::has($array, 'foo'));
        $this->assertFalse(Traverse::has($array, 'foo.bar'));
        $this->assertFalse(Traverse::has($array, 'empty.key'));
    }

    public function testInject() {
        $map = $this->map;
        $this->assertEquals($map, Traverse::inject($map, 'one.depth', 2));

        $result = Traverse::inject($map, 'one.foo', 'bar');
        $map['one']['foo'] = 'bar';
        $this->assertEquals($map, $result);
    }

    public function testInjectArray() {
        $array = $this->array;
        $this->assertEquals($array, Traverse::inject($array, 'one.depth', 2));

        $result = Traverse::inject($array, 'one.foo', 'bar');
        $array['one']['foo'] = 'bar';
        $this->assertEquals($array, $result);
    }

    public function testInsert() {
        $map = Map {};

        foreach ($this->collapsed as $key => $value) {
            $map = Traverse::insert($map, $key, $value);
        }

        $expected = $this->map;
        $expected->set('vector', null);

        $this->assertEquals($expected, $map);
    }

    public function testInsertArray() {
        $array = [];

        foreach ($this->collapsed as $key => $value) {
            $array = Traverse::insert($array, $key, $value);
        }

        $expected = $this->array;
        $expected['vector'] = null;

        $this->assertEquals($expected, $array);
    }

    public function testIsAlpha() {
        $this->assertTrue(Traverse::isAlpha(['foo', 'bar']));
        $this->assertTrue(Traverse::isAlpha(Vector {'foo', 'bar'}));
        $this->assertTrue(Traverse::isAlpha(Map {'foo' => 'bar', 'number' => '123'}, false));
        $this->assertTrue(Traverse::isAlpha(Vector {'bar', '123'}, false));

        $this->assertFalse(Traverse::isAlpha(Map {'foo' => 'bar', 'number' => '123'}));
        $this->assertFalse(Traverse::isAlpha(Vector {'bar', '123'}));
        $this->assertFalse(Traverse::isAlpha(Map {'foo' => 123}));
        $this->assertFalse(Traverse::isAlpha(Vector {null}));
        $this->assertFalse(Traverse::isAlpha(Vector {true}));
        $this->assertFalse(Traverse::isAlpha(Vector {false}));
        $this->assertFalse(Traverse::isAlpha(Vector {Vector {}}));
        $this->assertFalse(Traverse::isAlpha(Vector {new stdClass()}));
    }

    public function testIsNumeric() {
        $this->assertTrue(Traverse::isNumeric(['123', 456]));
        $this->assertTrue(Traverse::isNumeric(Vector {'123', 456}));
        $this->assertTrue(Traverse::isNumeric(Map {'foo' => 123, 'number' => '456'}));

        $this->assertFalse(Traverse::isNumeric(Vector {'foo', 'bar'}));
        $this->assertFalse(Traverse::isNumeric(Map {'foo' => 'bar', 'number' => '123'}));
        $this->assertFalse(Traverse::isNumeric(Vector {'bar', '123'}));
        $this->assertFalse(Traverse::isNumeric(Vector {null}));
        $this->assertFalse(Traverse::isNumeric(Vector {true}));
        $this->assertFalse(Traverse::isNumeric(Vector {false}));
        $this->assertFalse(Traverse::isNumeric(Vector {Vector {}}));
        $this->assertFalse(Traverse::isNumeric(Vector {new stdClass()}));
    }

    public function testKeyOf() {
        $map = $this->map;

        $this->assertEquals(null, Traverse::keyOf($map, 'fake'));
        $this->assertEquals('boolean', Traverse::keyOf($map, true));
        $this->assertEquals('one.two.three.depth', Traverse::keyOf($map, 3));
    }

    public function testKeyOfArray() {
        $array = $this->array;

        $this->assertEquals(null, Traverse::keyOf($array, 'fake'));
        $this->assertEquals('boolean', Traverse::keyOf($array, true));
        $this->assertEquals('one.two.three.depth', Traverse::keyOf($array, 3));
    }

    public function testMerge() {
        $one = Map {'one' => 1};
        $two = Map {'two' => 2};

        $this->assertEquals(Map {
            'one' => 1,
            'two' => 2
        }, Traverse::merge($one, $two));

        $two['one'] = 'one';

        $this->assertEquals(Map {
            'one' => 'one',
            'two' => 2
        }, Traverse::merge($one, $two));
    }

    public function testMergeArray() {
        $one = ['one' => 1];
        $two = ['two' => 2];

        $this->assertEquals([
            'one' => 1,
            'two' => 2
        ], Traverse::merge($one, $two));

        $two['one'] = 'one';

        $this->assertEquals([
            'one' => 'one',
            'two' => 2
        ], Traverse::merge($one, $two));
    }

    public function testMergeVectors() {
        $one = Vector {'foo', 'bar'};
        $two = Vector {'baz'};

        $this->assertEquals(Vector {'foo', 'bar', 'baz'}, Traverse::merge($one, $two));

        $two[] = 'boo';

        $this->assertEquals(Vector {'foo', 'bar', 'baz', 'baz', 'boo'}, Traverse::merge($one, $two));
    }

    public function testMergeNestedCollections() {
        $one = Map {'one' => 1, 'three' => ['foo' => 'bar', 'beep' => 'boop'], 'four' => Vector {1, 2, 3}};
        $two = Map {'two' => 2, 'three' => ['foo' => 'baz'], 'four' => Vector {4}};

        $this->assertEquals(Map {
            'one' => 1,
            'two' => 2,
            'three' => [
                'foo' => 'baz',
                'beep' => 'boop'
            ],
            'four' => Vector {1, 2, 3, 4}
        }, Traverse::merge($one, $two));
    }

    public function testPluck() {
        $data = Vector {
            Map {'name' => 'Miles', 'user' => Map {'id' => 1}},
            Map {'name' => 'Foo', 'user' => Map {'id' => 2}},
            Map {'key' => 'value', 'user' => Map {'id' => 3}},
            Map {'name' => 'Bar', 'user' => Map {'id' => 4}},
            Map {'name' => 'Baz', 'user' => Map {'id' => 5}},
        };

        $this->assertEquals(Vector {'Miles', 'Foo', 'Bar', 'Baz'}, Traverse::pluck($data, 'name'));
        $this->assertEquals(Vector {1, 2, 3, 4, 5}, Traverse::pluck($data, 'user.id'));
    }

    public function testPluckArray() {
        $data = [
            ['name' => 'Miles', 'user' => ['id' => 1]],
            ['name' => 'Foo', 'user' => ['id' => 2]],
            ['key' => 'value', 'user' => ['id' => 3]],
            ['name' => 'Bar', 'user' => ['id' => 4]],
            ['name' => 'Baz', 'user' => ['id' => 5]],
        ];

        $this->assertEquals(Vector {'Miles', 'Foo', 'Bar', 'Baz'}, Traverse::pluck($data, 'name'));
        $this->assertEquals(Vector {1, 2, 3, 4, 5}, Traverse::pluck($data, 'user.id'));
    }

    public function testReduce() {
        $this->assertEquals(Map {'boolean' => true, 'integer' => 123}, Traverse::reduce($this->map, Vector {'boolean', 'integer', 'foobar'}));
    }

    public function testReduceArray() {
        $this->assertEquals(Map {'boolean' => true, 'integer' => 123}, Traverse::reduce($this->array, Vector {'boolean', 'integer', 'foobar'}));
    }

    public function testRemove() {
        $map = $this->map;

        Traverse::remove($map, 'vector');
        Traverse::remove($map, 'one.depth');
        Traverse::remove($map, 'one.two.depth');

        $this->assertEquals(Map {
            'integer' => 123,
            'number' => '456',
            'string' => 'foobar',
            'boolean' => true,
            'one' => Map {
                'two' => Map {
                    'three' => Map {
                        'depth' => 3
                    }
                }
            }
        }, $map);

        Traverse::remove($map, 'one');

        $this->assertEquals(Map {
            'integer' => 123,
            'number' => '456',
            'string' => 'foobar',
            'boolean' => true
        }, $map);
    }

    public function testRemoveArray() {
        $array = $this->array;
        $array = Traverse::remove($array, 'vector');
        $array = Traverse::remove($array, 'one.depth');
        $array = Traverse::remove($array, 'one.two.depth');

        $this->assertEquals([
            'integer' => 123,
            'number' => '456',
            'string' => 'foobar',
            'boolean' => true,
            'one' => [
                'two' => [
                    'three' => [
                        'depth' => 3
                    ]
                ]
            ]
        ], $array);

        $array = Traverse::remove($array, 'one');

        $this->assertEquals([
            'integer' => 123,
            'number' => '456',
            'string' => 'foobar',
            'boolean' => true
        ], $array);
    }

    public function testSet() {
        $map = $this->map;

        Traverse::set($map, 'pair', Pair {'a', 'b'});
        Traverse::set($map, Map {
            'set' => Set {},
            'one.two.three.four.depth' => 4
        });

        $this->assertEquals(Map {
            'integer' => 123,
            'number' => '456',
            'string' => 'foobar',
            'boolean' => true,
            'vector' => Vector {},
            'one' => Map {
                'depth' => 1,
                'two' => Map {
                    'depth' => 2,
                    'three' => Map {
                        'depth' => 3,
                        'four' => Map {
                            'depth' => 4
                        }
                    }
                }
            },
            'pair' => Pair {'a', 'b'},
            'set' => Set {}
        }, $map);
    }

    public function testSetArray() {
        $array = $this->array;

        $array = Traverse::set($array, [
            'one.two.three.four.depth' => 4
        ]);

        $this->assertEquals([
            'integer' => 123,
            'number' => '456',
            'string' => 'foobar',
            'boolean' => true,
            'vector' => [],
            'one' => [
                'depth' => 1,
                'two' => [
                    'depth' => 2,
                    'three' => [
                        'depth' => 3,
                        'four' => [
                            'depth' => 4
                        ]
                    ]
                ]
            ]
        ], $array);
    }

    public function testSome() {
        $this->assertTrue(Traverse::some(Vector {123, 'abc', true, null}, function($value, $key) {
            return is_string($value);
        }));

        $this->assertFalse(Traverse::some(Vector {123, true, null}, function($value, $key) {
            return is_string($value);
        }));
    }

    public function testSomeArray() {
        $this->assertTrue(Traverse::some([123, 'abc', true, null], function($value, $key) {
            return is_string($value);
        }));

        $this->assertFalse(Traverse::some([123, true, null], function($value, $key) {
            return is_string($value);
        }));
    }

}