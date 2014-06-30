<?hh
namespace Titon\Utility;

use Titon\Test\TestCase;
use \Exception;
use \stdClass;

class TraverseTest extends TestCase {

    protected Map<string, mixed> $expanded = Map {
        'boolean' => true,
        'integer' => 123,
        'strings' => 'foobar',
        'numeric' => '1988',
        'empty' => Vector {},
        'one' => Map {
            'depth' => 1,
            'two' => Map {
                'depth' => 2,
                'three' => Map {
                    'depth' => 3,
                    'false' => false,
                    'true' => true,
                    'null' => null,
                    'zero' => 0,
                    'four' => Map {
                        'five' => Map {
                            'six' => Map {
                                'seven' => Map {
                                    'key' => 'We can go deeper!'
                                }
                            }
                        }
                    }
                }
            }
        }
    };

    protected Map<string, mixed> $collapsed = Map {
        'boolean' => true,
        'integer' => 123,
        'strings' => 'foobar',
        'numeric' => '1988',
        'empty' => Vector {},
        'one.depth' => 1,
        'one.two.depth' => 2,
        'one.two.three.depth' => 3,
        'one.two.three.false' => false,
        'one.two.three.true' => true,
        'one.two.three.null' => null,
        'one.two.three.zero' => 0,
        'one.two.three.four.five.six.seven.key' => 'We can go deeper!'
    };

    public function testDepth() {
        $map = Map {'one' => Map {'two' => Map {'three' => Map {'four' => Map {'five' => 5}}}}};
        $vector = Vector {Vector {Vector {3}}};
        $set = Set {1};
        // Doesn't work with Pair

        $this->assertEquals(0, Traverse::depth(Vector {}));
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
            'array' => Vector {
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
            'array' => Vector {
                246,
                912,
                'FOOBAR',
                true
            }
        }, Traverse::each($data, $callback, true));
    }

    public function testEvery() {
        $callback = function($value, $key) {
            return is_int($value);
        };

        $this->assertTrue(Traverse::every(Vector {123, 456}, $callback));
        $this->assertFalse(Traverse::every(Vector {123, '456'}, $callback));

        $this->assertTrue(Traverse::every(Map {'a' => 123, 'b' => 456}, $callback));
        $this->assertFalse(Traverse::every(Map {'a' => 123, 'b' => '456'}, $callback));
    }

    public function testExclude() {
        $this->assertEquals(Map {'foo' => 123}, Traverse::exclude(Map {'foo' => 123, 'bar' => 456, 'baz' => 789}, ['bar', 'baz']));
    }

    public function testExpand() {
        $this->assertEquals(Map {
            'foo' => 'bar',
            'one' => Map {
                'two' => Map {}
            }
        }, Traverse::expand(Map {
            'foo' => 'bar',
            'one.two' => Map {}
        }));
    }

    public function testExtract() {
        $map = Map {
            'one' => Map {
                'key' => 1,
                'two' => Map {
                    'key' => 2,
                    'three' => Map {
                        'key' => 3,
                        'four' => Map {}
                    }
                }
            }
        };

        $this->assertEquals(null, Traverse::extract($map, ''));
        $this->assertEquals(null, Traverse::extract($map, 'fake.path'));
        $this->assertEquals(2, Traverse::extract($map, 'one.two.key'));
        $this->assertEquals(null, Traverse::extract($map, 'one.two.three.four.key'));
        $this->assertEquals(Map {
            'key' => 3,
            'four' => Map {}
        }, Traverse::extract($map, 'one.two.three'));
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
        $this->assertEquals(Map {
            'foo' => 'bar',
            'one.two.key' => 2,
            'one.two.vector' => null,
        }, Traverse::flatten(Map {
            'foo' => 'bar',
            'one' => Map {
                'two' => Map {
                    'key' => 2,
                    'vector' => Vector {}
                }
            }
        }));
    }

    public function testFlattenPrefix() {
        $this->assertEquals(Map {
            'titon.foo' => 'bar',
            'titon.one.two.key' => 2,
            'titon.one.two.vector' => null,
        }, Traverse::flatten(Map {
            'foo' => 'bar',
            'one' => Map {
                'two' => Map {
                    'key' => 2,
                    'vector' => Vector {}
                }
            }
        }, 'titon'));
    }

    public function testGet() {
        $map = Map {
            'one' => Map {
                'key' => 1,
                'two' => Map {
                    'key' => 2,
                    'three' => Map {
                        'key' => 3,
                        'four' => Map {}
                    }
                }
            }
        };

        $this->assertEquals($map, Traverse::get($map));
        $this->assertEquals(Map {
            'key' => 3,
            'four' => Map {}
        }, Traverse::get($map, 'one.two.three'));
    }

    public function testHas() {
        $data = $this->expanded;

        $this->assertTrue(Traverse::has($data, 'boolean'));
        $this->assertTrue(Traverse::has($data, 'empty'));
        $this->assertTrue(Traverse::has($data, 'one.depth'));
        $this->assertTrue(Traverse::has($data, 'one.two.depth'));
        $this->assertTrue(Traverse::has($data, 'one.two.three.false'));
        $this->assertTrue(Traverse::has($data, 'one.two.three.true'));
        $this->assertTrue(Traverse::has($data, 'one.two.three.four.five.six.seven.key'));
        $this->assertTrue(Traverse::has($data, 'one.two.three.null'));

        $this->assertFalse(Traverse::has($data, ''));
        $this->assertFalse(Traverse::has(Map {}, 'foo'));
        $this->assertFalse(Traverse::has($data, 'one.two.three.some.really.deep.depth'));
        $this->assertFalse(Traverse::has($data, 'foo'));
        $this->assertFalse(Traverse::has($data, 'foo.bar'));
        $this->assertFalse(Traverse::has($data, 'empty.key'));
    }

    public function testInject() {
        $data = $this->expanded;
        $this->assertEquals($data, Traverse::inject($data, 'one.depth', 2));

        $result = Traverse::inject($data, 'one.foo', 'bar');
        $data['one']['foo'] = 'bar';
        $this->assertEquals($data, $result);
    }

    public function testInsert() {
        $data = Map {};

        foreach ($this->collapsed as $key => $value) {
            $data = Traverse::insert($data, $key, $value);
        }

        $this->assertEquals($this->expanded, $data);
        $this->assertEquals(Vector{'value'}, Traverse::insert(Vector {}, '', 'value'));
    }

    public function testIsAlpha() {
        $this->assertTrue(Traverse::isAlpha(Vector{'foo', 'bar'}));
        $this->assertTrue(Traverse::isAlpha(Map{'foo' => 'bar', 'number' => '123'}, false));
        $this->assertTrue(Traverse::isAlpha(Vector{'bar', '123'}, false));

        $this->assertFalse(Traverse::isAlpha(Map{'foo' => 'bar', 'number' => '123'}));
        $this->assertFalse(Traverse::isAlpha(Vector{'bar', '123'}));
        $this->assertFalse(Traverse::isAlpha(Map{'foo' => 123}));
        $this->assertFalse(Traverse::isAlpha(Vector{null}));
        $this->assertFalse(Traverse::isAlpha(Vector{true}));
        $this->assertFalse(Traverse::isAlpha(Vector{false}));
        $this->assertFalse(Traverse::isAlpha(Vector{Vector{}}));
        $this->assertFalse(Traverse::isAlpha(Vector{new stdClass()}));
    }

    public function testIsNumeric() {
        $this->assertTrue(Traverse::isNumeric(Vector{'123', 456}));
        $this->assertTrue(Traverse::isNumeric(Map{'foo' => 123, 'number' => '456'}));

        $this->assertFalse(Traverse::isNumeric(Vector{'foo', 'bar'}));
        $this->assertFalse(Traverse::isNumeric(Map{'foo' => 'bar', 'number' => '123'}));
        $this->assertFalse(Traverse::isNumeric(Vector{'bar', '123'}));
        $this->assertFalse(Traverse::isNumeric(Vector{null}));
        $this->assertFalse(Traverse::isNumeric(Vector{true}));
        $this->assertFalse(Traverse::isNumeric(Vector{false}));
        $this->assertFalse(Traverse::isNumeric(Vector{Vector{}}));
        $this->assertFalse(Traverse::isNumeric(Vector{new stdClass()}));
    }

    public function testKeyOf() {
        $data = $this->expanded;

        $this->assertEquals(null, Traverse::keyOf($data, 'fakeValue'));
        $this->assertEquals('boolean', Traverse::keyOf($data, true));
        $this->assertEquals('one.two.three.depth', Traverse::keyOf($data, 3));
    }

    public function testMerge() {
        $data1 = [
            'foo' => 'bar',
            'boolean' => true,
            'string' => 'abc',
            'number' => 123,
            'one'
        ];

        $data2 = [
            'foo' => 'baz',
            'boolean' => false,
            'string' => 'xyz',
            'number' => 456,
            'two'
        ];

        $this->assertEquals([
            'foo' => 'baz',
            'boolean' => false,
            'string' => 'xyz',
            'number' => 456,
            'two'
        ], Traverse::merge($data1, $data2));

        $data1['array'] = [
            'key' => 'value',
            123,
            true
        ];

        $data2['array'] = [];

        $this->assertEquals([
            'foo' => 'baz',
            'boolean' => false,
            'string' => 'xyz',
            'number' => 456,
            'two',
            'array' => [
                'key' => 'value',
                123,
                true
            ]
        ], Traverse::merge($data1, $data2));

        $data2['array'] = [
            'key' => 'base',
            'foo' => 'bar',
            123
        ];

        $this->assertEquals([
            'foo' => 'bar',
            'boolean' => true,
            'string' => 'abc',
            'number' => 123,
            'one',
            'array' => [
                'key' => 'value',
                'foo' => 'bar',
                123,
                true
            ]
        ], Traverse::merge($data2, $data1));

        $this->assertEquals([], Traverse::merge());
    }

    public function testOverwrite() {
        $data1 = [
            'foo' => 'bar',
            123,
            'array' => [
                'boolean' => true,
                'left' => 'left'
            ]
        ];

        $data2 = [
            'foo' => 'baz',
            456,
            'array' => [
                'boolean' => false,
                'right' => 'right'
            ]
        ];

        $this->assertEquals([
            'foo' => 'baz',
            456,
            'array' => [
                'boolean' => false,
                'left' => 'left'
            ]
        ], Traverse::overwrite($data1, $data2));
    }

    public function testPluck() {
        $data = Vector {
            Map { 'name' => 'Miles', 'user' => Map { 'id' => 1}},
            Map { 'name' => 'Foo', 'user' => Map { 'id' => 2}},
            Map { 'key' => 'value', 'user' => Map { 'id' => 3}},
            Map { 'name' => 'Bar', 'user' => Map { 'id' => 4}},
            Map { 'name' => 'Baz', 'user' => Map { 'id' => 5}},
        };

        $this->assertEquals(Vector {'Miles', 'Foo', 'Bar', 'Baz'}, Traverse::pluck($data, 'name'));
        $this->assertEquals(Vector {1, 2, 3, 4, 5}, Traverse::pluck($data, 'user.id'));
    }

    public function testRange() {
        $this->assertEquals([0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10], Traverse::range(0, 10));
        $this->assertEquals([0 => 0, 2 => 2, 4 => 4, 6 => 6, 8 => 8, 10 => 10], Traverse::range(0, 10, 2));
        $this->assertEquals([0 => 0, 3 => 3, 6 => 6, 9 => 9], Traverse::range(0, 10, 3));
        $this->assertEquals([0 => 0, 13 => 13, 26 => 26, 39 => 39, 52 => 52, 65 => 65, 78 => 78, 91 => 91], Traverse::range(0, 100, 13));
        $this->assertEquals([23 => 23, 29 => 29, 35 => 35, 41 => 41, 47 => 47, 53 => 53, 59 => 59, 65 => 65], Traverse::range(23, 66, 6));

        $this->assertEquals([0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10], Traverse::range(0, 10, 1, false));
        $this->assertEquals([0, 2, 4, 6, 8, 10], Traverse::range(0, 10, 2, false));
        $this->assertEquals([0, 3, 6, 9], Traverse::range(0, 10, 3, false));
        $this->assertEquals([0, 13, 26, 39, 52, 65, 78, 91], Traverse::range(0, 100, 13, false));
        $this->assertEquals([23, 29, 35, 41, 47, 53, 59, 65], Traverse::range(23, 66, 6, false));

        // reverse
        $this->assertEquals([5 => 5, 4 => 4, 3 => 3, 2 => 2, 1 => 1, 0 => 0], Traverse::range(5, 0, 1));
        $this->assertEquals([10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0], Traverse::range(10, 0, 1, false));
        $this->assertEquals([65, 59, 53, 47, 41, 35, 29, 23], Traverse::range(65, 23, 6, false));
    }

    public function testReduce() {
        $this->assertEquals(['boolean' => true, 'integer' => 123], Traverse::reduce($this->expanded, ['boolean', 'integer', 'foobar']));
    }

    public function testRemove() {
        $data = $this->expanded;
        $match = $data;

        unset($match['boolean']);
        $data = Traverse::remove($data, 'boolean');
        $this->assertEquals($match, $data);

        unset($match['one']['depth']);
        $data = Traverse::remove($data, 'one.depth');
        $this->assertEquals($match, $data);

        unset($match['one']['two']['depth']);
        $data = Traverse::remove($data, 'one.two.depth');
        $this->assertEquals($match, $data);

        unset($match['one']['two']['three']['depth'], $match['one']['two']['three']['zero'], $match['one']['two']['three']['null']);
        $data = Traverse::remove($data, 'one.two.three.depth');
        $data = Traverse::remove($data, 'one.two.three.zero');
        $data = Traverse::remove($data, 'one.two.three.null');
        $this->assertEquals($match, $data);

        unset($match['one']['two']['three']['four']['five']['six']['seven']['key']);
        $data = Traverse::remove($data, 'one.two.three.four.five.six.seven.key');
        $this->assertEquals($match, $data);

        $data = Traverse::remove($data, 'a.fake.path');
        $this->assertEquals($match, $data);
    }

    public function testSet() {
        $data = $this->expanded;
        $match = $data;

        $data = Traverse::set($data, 'key', 'value');
        $match['key'] = 'value';
        $this->assertEquals($match, $data);

        $data = Traverse::set($data, 'key.key', 'value');
        $match['key'] = ['key' => 'value'];
        $this->assertEquals($match, $data);

        $data = Traverse::set($data, Map {
            'key.key.key' => 'value',
            'true' => true,
            'one.false' => false
        });
        $match['key']['key'] = Map {'key' => 'value'};
        $match['true']= true;
        $match['one']['false'] = false;
        $this->assertEquals($match, $data);
    }

    public function testSome() {
        $this->assertTrue(Traverse::some(Vector {123, 'abc', true, null}, function($value, $key) {
            return is_string($value);
        }));

        $this->assertFalse(Traverse::some(Vector {123, true, null}, function($value, $key) {
            return is_string($value);
        }));
    }

}