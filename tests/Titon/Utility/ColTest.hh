<?hh // strict
namespace Titon\Utility;

use Titon\Test\TestCase;

class ColTest extends TestCase {

    public function testDepth(): void {
        $this->assertEquals(0, Col::depth(Map {}));
        $this->assertEquals(1, Col::depth(Map {'one' => 1}));
        $this->assertEquals(3, Col::depth(Map {'one' => Map {'two' => Map {'three' => 3}}}));
        $this->assertEquals(4, Col::depth(Map {'one' => ['two' => Map {'three' => Vector {4}}]}));
    }

    public function testEachMap(): void {
        $this->assertEquals(Map {
            1 => 'FOO',
            2 => 123,
            3 => 'BAR'
        }, Col::each(Map {
            1 => 'foo',
            2 => 123,
            3 => 'bar'
        }, function($key, $value): void {
            return is_string($value) ? strtoupper($value) : $value;
        }));
    }

    public function testEachVector(): void {
        $this->assertEquals(Vector {
            'FOO',
            123,
            'BAR'
        }, Col::each(Vector {
            'foo',
            123,
            'bar'
        }, function($key, $value): void {
            return is_string($value) ? strtoupper($value) : $value;
        }));
    }

    public function testEveryMap(): void {
        $callback = function($key, $value): void {
            return is_int($value);
        };

        $this->assertTrue(Col::every(Map {1 => 123, 2 => 456, 3 => 789}, $callback));
        $this->assertFalse(Col::every(Map {1 => 123, 2 => 456, 3 => '789'}, $callback));
    }

    public function testEveryVector(): void {
        $callback = function($key, $value): void {
            return is_int($value);
        };

        $this->assertTrue(Col::every(Vector {123, 456, 789}, $callback));
        $this->assertFalse(Col::every(Vector {123, 456, '789'}, $callback));
    }

    public function testExclude(): void {
        $this->assertEquals(Map {1 => 'bar'}, Col::exclude(Map {0 => 'foo', 1 => 'bar'}, Vector {0}));
    }

    public function testExpand(): void {
        $this->assertEquals(Map {
            'app' => Map {
                'name' => 'Titon',
                'paths' => Map {
                    'root' => '/'
                }
            },
            'env' => Map {
                'host' => 'titon.io'
            }
        }, Col::expand(Map {
            'app.name' => 'Titon',
            'app.paths.root' => '/',
            'env.host' => 'titon.io'
        }));
    }

    public function testExtract(): void {
        $map = Map {
            'number' => 123,
            'string' => 'foo',
            'one' => Map {
                'number' => 456,
                'string' => 'bar',
                'two' => Map {
                    'number' => 789,
                    'string' => 'baz'
                }
            }
        };

        $this->assertEquals(123, Col::extract($map, 'number'));
        $this->assertEquals(456, Col::extract($map, 'one.number'));
        $this->assertEquals(789, Col::extract($map, 'one.two.number'));
        $this->assertEquals('bar', Col::extract($map, 'one.string'));
        $this->assertEquals(null, Col::extract($map, 'one.two.three.boolean'));
        $this->assertEquals(Map {
            'number' => 789,
            'string' => 'baz'
        }, Col::extract($map, 'one.two'));
    }

    public function testFlatten(): void {
        $this->assertEquals(Map {
            'app.name' => 'Titon',
            'app.paths.root' => '/',
            'env.host' => 'titon.io'
        }, Col::flatten(Map {
            'app' => Map {
                'name' => 'Titon',
                'paths' => Map {
                    'root' => '/'
                }
            },
            'env' => Map {
                'host' => 'titon.io'
            }
        }));
    }

    public function testGet(): void {
        $map = Map {
            'number' => 123,
            'string' => 'foo'
        };

        $this->assertEquals(123, Col::get($map, 'number'));
        $this->assertEquals(null, Col::get($map, 'boolean'));
        $this->assertEquals(false, Col::get($map, 'boolean', false));
        $this->assertEquals(Map {
            'number' => 123,
            'string' => 'foo'
        }, Col::get($map, ''));
    }

    public function testHas(): void {
        $map = Map {
            'number' => 123,
            'string' => 'foo',
            'one' => Map {
                'number' => 456,
                'string' => 'bar',
                'two' => Map {
                    'number' => 789,
                    'string' => 'baz'
                }
            }
        };

        $this->assertTrue(Col::has($map, 'number'));
        $this->assertTrue(Col::has($map, 'one.string'));
        $this->assertTrue(Col::has($map, 'one.two'));
        $this->assertFalse(Col::has($map, 'one.two.three'));
    }

    public function testInject(): void {
        $map = Map {
            'number' => 123,
            'string' => 'foo',
        };

        $map = Col::inject($map, 'boolean', true);

        $this->assertEquals(Map {
            'number' => 123,
            'string' => 'foo',
            'boolean' => true
        }, $map);

        $map = Col::inject($map, 'boolean', false);

        $this->assertEquals(Map {
            'number' => 123,
            'string' => 'foo',
            'boolean' => true
        }, $map);
    }

    public function testInsert(): void {
        $map = Map {
            'number' => 123,
            'string' => 'foo',
        };

        $map = Col::insert($map, 'boolean', true);

        $this->assertEquals(Map {
            'number' => 123,
            'string' => 'foo',
            'boolean' => true
        }, $map);

        $map = Col::insert($map, 'one.two.three.boolean', false);

        $this->assertEquals(Map {
            'number' => 123,
            'string' => 'foo',
            'boolean' => true,
            'one' => Map {
                'two' => Map {
                    'three' => Map {
                        'boolean' => false
                    }
                }
            }
        }, $map);

        $map = Col::insert($map, 'one.two.vector', Vector {1, 2, 3});

        $this->assertEquals(Map {
            'number' => 123,
            'string' => 'foo',
            'boolean' => true,
            'one' => Map {
                'two' => Map {
                    'three' => Map {
                        'boolean' => false
                    },
                    'vector' => Vector {1, 2, 3}
                }
            }
        }, $map);
    }

    public function testIsAlphaMap(): void {
        $map = Map {
            1 => 'foo',
            2 => 'bar',
            3 => '123'
        };

        $this->assertTrue(Col::isAlpha($map, false));
        $this->assertFalse(Col::isAlpha($map));
    }

    public function testIsAlphaVector(): void {
        $vector = Vector {
            'foo',
            'bar',
            '123'
        };

        $this->assertTrue(Col::isAlpha($vector, false));
        $this->assertFalse(Col::isAlpha($vector));
    }

    public function testIsNumericMap(): void {
        $this->assertTrue(Col::isNumeric(Map {
            1 => 123,
            2 => '456',
            3 => 789.10
        }));

        $this->assertFalse(Col::isNumeric(Map {
            1 => 123,
            2 => 'one',
            3 => 789.10
        }));
    }

    public function testIsNumericVectorp(): void {
        $this->assertTrue(Col::isNumeric(Vector {
            123,
            '456',
            789.10
        }));

        $this->assertFalse(Col::isNumeric(Vector {
            123,
            'one',
            789.10
        }));
    }

    public function testKeyOfMap(): void {
        $map = Map {
            'number' => 123,
            'string' => 'foo',
            'one' => Map {
                'number' => 456,
                'string' => 'bar',
                'two' => Map {
                    'number' => 789,
                    'string' => 'baz'
                }
            }
        };

        $this->assertEquals('string', Col::keyOf($map, 'foo'));
        $this->assertEquals('one.two.number', Col::keyOf($map, 789));
    }

    public function testKeyOfVector(): void {
        $vector = Vector {
            123,
            'foo',
            Vector {
                456,
                'bar',
                Map {
                    'number' => 789,
                    'string' => 'baz'
                }
            }
        };

        $this->assertEquals('1', Col::keyOf($vector, 'foo'));
        $this->assertEquals('2.2.number', Col::keyOf($vector, 789));
    }

    public function testMerge(): void {
        $one = Map {
            'a' => 1,
            'b' => 2,
            'c' => 3
        };

        $two = Map {
            'a' => 10
        };

        $this->assertEquals(Map {
            'a' => 10,
            'b' => 2,
            'c' => 3
        }, Col::merge($one, $two));
    }

    public function testMergeMultiple(): void {
        $one = Map {
            'a' => 1,
            'b' => 2,
            'c' => 3
        };

        $two = Map {
            'a' => 10
        };

        $three = Map {
            'a' => 100,
            'c' => 30
        };

        $this->assertEquals(Map {
            'a' => 100,
            'b' => 2,
            'c' => 30
        }, Col::merge($one, $two, $three));
    }

    public function testMergeRecursive(): void {
        $one = Map {
            'a' => 1,
            'b' => Map {
                'c' => 2,
                'd' => 3
            }
        };

        $two = Map {
            'a' => 10,
            'b' => Map {
                'c' => 20
            },
            'e' => Map {
                'f' => 5
            }
        };

        $this->assertEquals(Map {
            'a' => 10,
            'b' => Map {
                'c' => 20,
                'd' => 3
            },
            'e' => Map {
                'f' => 5
            }
        }, Col::merge($one, $two));
    }

    public function testMergeWithVector(): void {
        $one = Map {
            'a' => 1,
            'b' => 2,
            'c' => Vector {1, 2, 3}
        };

        $two = Map {
            'c' => Vector {4, 5, 6}
        };

        $this->assertEquals(Map {
            'a' => 1,
            'b' => 2,
            'c' => Vector {1, 2, 3, 4, 5, 6}
        }, Col::merge($one, $two));
    }

    public function testPluck(): void {
        $map = Map {
            0 => Map {'name' => 'foo'},
            1 => Map {'key' => 'value'},
            2 => Map {'name' => 'bar'},
            3 => Map {'key' => 'var'},
        };

        $this->assertEquals(Vector {'foo', 'bar'}, Col::pluck($map, 'name'));

        $map = Map {
            0 => Map {'meta' => Map {'id' => 1}},
            1 => Map {'meta' => Map {'id' => 2}},
            2 => Map {'meta' => Map {'id' => 3}},
        };

        $this->assertEquals(Vector {1, 2, 3}, Col::pluck($map, 'meta.id'));
    }

    public function testReduce(): void {
        $map = Map {
            'number' => 123,
            'string' => 'foo',
            'boolean' => true,
            'float' => 456.789
        };

        $this->assertEquals(Map {
            'number' => 123,
            'float' => 456.789
        }, Col::reduce($map, Vector {'number', 'float'}));
    }

    public function testRemove(): void {
        $map = Map {
            'number' => 123,
            'string' => 'foo',
            'one' => Map {
                'number' => 456,
                'string' => 'bar',
                'two' => Map {
                    'number' => 789,
                    'string' => 'baz'
                }
            }
        };

        $map = Col::remove($map, 'one.string');

        $this->assertEquals(Map {
            'number' => 123,
            'string' => 'foo',
            'one' => Map {
                'number' => 456,
                'two' => Map {
                    'number' => 789,
                    'string' => 'baz'
                }
            }
        }, $map);

        $map = Col::remove($map, 'one.two');

        $this->assertEquals(Map {
            'number' => 123,
            'string' => 'foo',
            'one' => Map {
                'number' => 456
            }
        }, $map);

        $map = Col::remove($map, 'one.two.three');

        $this->assertEquals(Map {
            'number' => 123,
            'string' => 'foo',
            'one' => Map {
                'number' => 456
            }
        }, $map);
    }

    public function testSet(): void {
        $map = Map {};

        $map = Col::set($map, 'string', 'foo');

        $this->assertEquals(Map {
            'string' => 'foo'
        }, $map);

        $map = Col::set($map, Map {
            'one.two.number' => 123,
            'a.b.boolean' => true
        });

        $this->assertEquals(Map {
            'string' => 'foo',
            'one' => Map {
                'two' => Map {
                    'number' => 123
                }
            },
            'a' => Map {
                'b' => Map {
                    'boolean' => true
                }
            }
        }, $map);
    }

    public function testSomeMap(): void {
        $this->assertTrue(Col::some(Map {0 => 'foo', 1 => 123, 2 => true}, function($key, $value): void {
            return is_numeric($value);
        }));

        $this->assertFalse(Col::some(Map {0 => 'foo', 1 => 123, 2 => true}, function($key, $value): void {
            return is_float($value);
        }));
    }

    public function testSomeVector(): void {
        $this->assertTrue(Col::some(Vector {'foo', 123, true}, function($key, $value): void {
            return is_numeric($value);
        }));

        $this->assertFalse(Col::some(Vector {'foo', 123, true}, function($key, $value): void {
            return is_float($value);
        }));
    }

    public function testToArray(): void {
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
        ], Col::toArray($map));

        $this->assertEquals(['foo'], Col::toArray('foo'));
    }

    public function testToArrayRecursive(): void {
        $actual = Map {
            'foo' => 'bar',
            'array' => Vector {1, 2, 3},
            'keyed-array' => Map {'a' => 1, 'b' => 2, 'c' => 3},
            'vector' => Vector {'a', 'b', 'c'},
            'map' => Map {'a' => 1, 'b' => 2, 'c' => 3},
            'indexed-map' => Map {1 => 'a', 2 => 'b', 3 => 'c'},
            'nested' => Map {
                'array' => Vector {4, 5, 6},
                'nested-array' => Vector {
                    Map {},
                    Vector {}
                },
                'integer' => 123456,
                'boolean' => true
            }
        };

        $expected = [
            'foo' => 'bar',
            'array' => [1, 2, 3],
            'keyed-array' => ['a' => 1, 'b' => 2, 'c' => 3],
            'vector' => ['a', 'b', 'c'],
            'map' => ['a' => 1, 'b' => 2, 'c' => 3],
            'indexed-map' => [1 => 'a', 2 => 'b', 3 => 'c'],
            'nested' => [
                'array' => [4, 5, 6],
                'nested-array' => [
                    [],
                    []
                ],
                'integer' => 123456,
                'boolean' => true
            ]
        ];

        $this->assertEquals($expected, Col::toArray($actual));
    }

    public function testToMap(): void {
        $this->assertEquals(Map {}, Col::toMap(Vector {}));
        $this->assertEquals(Map {0 => 'foo'}, Col::toMap(Vector {'foo'}));
        $this->assertEquals(Map {}, Col::toMap([]));
        $this->assertEquals(Map {'foo' => 'bar'}, Col::toMap(['foo' => 'bar']));
        $this->assertEquals(Map {0 => 'foo'}, Col::toMap(['foo']));
        $this->assertEquals(Map {0 => 'foo'}, Col::toMap('foo'));
        $this->assertEquals(Map {0 => 123}, Col::toMap(123));
        $this->assertEquals(Map {}, Col::toMap(Map {}));
        $this->assertEquals(Map {'foo' => 'bar'}, Col::toMap(Map {'foo' => 'bar'}));
    }

    public function testToMapRecursive(): void {
        $actual = [
            'foo' => 'bar',
            'array' => [1, 2, 3],
            'keyed-array' => ['a' => 1, 'b' => 2, 'c' => 3],
            'vector' => Vector {'a', 'b', 'c'},
            'map' => Map {'a' => 1, 'b' => 2, 'c' => 3},
            'indexed-map' => Map {1 => 'a', 2 => 'b', 3 => 'c'},
            'nested' => Map {
                'array' => [4, 5, 6],
                'nested-array' => [
                    Map {},
                    Vector {}
                ],
                'integer' => 123456,
                'boolean' => true
            }
        ];

        $expected = Map {
            'foo' => 'bar',
            'array' => Vector {1, 2, 3},
            'keyed-array' => Map {'a' => 1, 'b' => 2, 'c' => 3},
            'vector' => Vector {'a', 'b', 'c'},
            'map' => Map {'a' => 1, 'b' => 2, 'c' => 3},
            'indexed-map' => Map {1 => 'a', 2 => 'b', 3 => 'c'},
            'nested' => Map {
                'array' => Vector {4, 5, 6},
                'nested-array' => Vector {
                    Map {},
                    Vector {}
                },
                'integer' => 123456,
                'boolean' => true
            }
        };

        $this->assertEquals($expected, Col::toMap($actual));
    }

    public function testToVector(): void {
        $this->assertEquals(Vector {}, Col::toVector(Vector {}));
        $this->assertEquals(Vector {'foo'}, Col::toVector(Vector {'foo'}));
        $this->assertEquals(Vector {}, Col::toVector([]));
        $this->assertEquals(Vector {'foo'}, Col::toVector(['foo']));
        $this->assertEquals(Vector {'foo'}, Col::toVector('foo'));
        $this->assertEquals(Vector {123}, Col::toVector(123));
        $this->assertEquals(Vector {}, Col::toVector(Map {}));
        $this->assertEquals(Vector {'bar'}, Col::toVector(Map {'foo' => 'bar'}));
    }

    public function testToVectorRecursive(): void {
        $actual = [
            'foo' => 'bar',
            'array' => [1, 2, 3],
            'keyed-array' => ['a' => 1, 'b' => 2, 'c' => 3],
            'vector' => Vector {'a', 'b', 'c'},
            'map' => Map {'a' => 1, 'b' => 2, 'c' => 3},
            'indexed-map' => Map {1 => 'a', 2 => 'b', 3 => 'c'},
            'nested' => Map {
            'array' => [4, 5, 6],
                'nested-array' => [
                Map {},
                    Vector {}
                ],
                'integer' => 123456,
                'boolean' => true
            }
        ];

        $expected = Vector {
            'bar',
            Vector {1, 2, 3},
            Map {'a' => 1, 'b' => 2, 'c' => 3},
            Vector {'a', 'b', 'c'},
            Map {'a' => 1, 'b' => 2, 'c' => 3},
            Map {1 => 'a', 2 => 'b', 3 => 'c'},
            Map {
                'array' => Vector {4, 5, 6},
                'nested-array' => Vector {
                    Map {},
                    Vector {}
                },
                'integer' => 123456,
                'boolean' => true
            }
        };

        $this->assertEquals($expected, Col::toVector($actual));
    }

}
