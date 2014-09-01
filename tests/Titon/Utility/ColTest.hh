<?hh
namespace Titon\Utility;

use Titon\Test\TestCase;

class ColTest extends TestCase {

    public function testDepthMap() {
        $this->assertEquals(0, Col::depth(Map {}));
        $this->assertEquals(1, Col::depth(Map {'one' => 1}));
        $this->assertEquals(3, Col::depth(Map {'one' => Map {'two' => Map {'three' => 3}}}));
        $this->assertEquals(4, Col::depth(Map {'one' => Map {'two' => Map {'three' => Vector {4}}}}));
    }

    public function testDepthVector() {
        $this->assertEquals(0, Col::depth(Vector {}));
        $this->assertEquals(1, Col::depth(Vector {1}));
        $this->assertEquals(3, Col::depth(Vector {Vector {Vector {3}}}));
        $this->assertEquals(5, Col::depth(Vector {Vector {Vector {Vector {Map {'five' => 5}}}}}));
    }

    public function testEachMap() {
        $this->assertEquals(Map {
            1 => 'FOO',
            2 => 123,
            3 => 'BAR'
        }, Col::each(Map {
            1 => 'foo',
            2 => 123,
            3 => 'bar'
        }, function($key, $value) {
            return is_string($value) ? strtoupper($value) : $value;
        }));
    }

    public function testEachMapRecursive() {
        $this->assertEquals(Map {
            1 => 'FOO',
            2 => 123,
            3 => 'BAR',
            4 => Map {
                1 => 'BAZ',
                2 => 456
            }
        }, Col::each(Map {
            1 => 'foo',
            2 => 123,
            3 => 'bar',
            4 => Map {
                1 => 'baz',
                2 => 456
            }
        }, function($key, $value) {
            return is_string($value) ? strtoupper($value) : $value;
        }));
    }

    public function testEachVector() {
        $this->assertEquals(Vector {
            'FOO',
            123,
            'BAR'
        }, Col::each(Vector {
            'foo',
            123,
            'bar'
        }, function($key, $value) {
            return is_string($value) ? strtoupper($value) : $value;
        }));
    }

    public function testEachVectorRecursive() {
        $this->assertEquals(Vector {
            'FOO',
            123,
            'BAR',
            Vector {
                'BAZ',
                456
            }
        }, Col::each(Vector {
            'foo',
            123,
            'bar',
            Vector {
                'baz',
                456
            }
        }, function($key, $value) {
            return is_string($value) ? strtoupper($value) : $value;
        }));
    }

    public function testEveryMap() {
        $callback = function($key, $value) {
            return is_int($value);
        };

        $this->assertTrue(Col::every(Map {1 => 123, 2 => 456, 3 => 789}, $callback));
        $this->assertFalse(Col::every(Map {1 => 123, 2 => 456, 3 => '789'}, $callback));
    }

    public function testEveryVector() {
        $callback = function($key, $value) {
            return is_int($value);
        };

        $this->assertTrue(Col::every(Vector {123, 456, 789}, $callback));
        $this->assertFalse(Col::every(Vector {123, 456, '789'}, $callback));
    }

    public function testExcludeMap() {
        $this->assertEquals(Map {1 => 'bar'}, Col::exclude(Map {0 => 'foo', 1 => 'bar'}, Vector {0}));
    }

    public function testExcludeVector() {
        $this->assertEquals(Vector {'bar'}, Col::exclude(Vector {'foo', 'bar'}, Vector {0}));
    }

    public function testExpand() {
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

    public function testExtractMap() {
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
    }

    public function testExtractVector() {
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

        $this->assertEquals(123, Col::extract($vector, '0'));
        $this->assertEquals(456, Col::extract($vector, '2.0'));
        $this->assertEquals(789, Col::extract($vector, '2.2.number'));
        $this->assertEquals('bar', Col::extract($vector, '2.1'));
        $this->assertEquals(null, Col::extract($vector, '3.0.1'));
    }

    public function testFilterMap() {
        $this->markTestSkipped('todo');
    }

    public function testFilterVector() {
        $this->markTestSkipped('todo');
    }

    public function testFlatten() {
        $this->markTestSkipped('todo - after merge()');

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

    public function testGetMap() {
        $map = Map {
            'number' => 123,
            'string' => 'foo'
        };

        $this->assertEquals(123, Col::get($map, 'number'));
        $this->assertEquals(null, Col::get($map, 'boolean'));
        $this->assertEquals(Map {
            'number' => 123,
            'string' => 'foo'
        }, Col::get($map, ''));
    }

    public function testGetVector() {
        $vector = Vector {
            123,
            'foo'
        };

        $this->assertEquals(123, Col::get($vector, '0'));
        $this->assertEquals(null, Col::get($vector, '3'));
        $this->assertEquals(Vector {
            123,
            'foo'
        }, Col::get($vector, ''));
    }

    public function testHasMap() {
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

    public function testHasVector() {
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

        $this->assertTrue(Col::has($vector, '0'));
        $this->assertTrue(Col::has($vector, '2.0'));
        $this->assertTrue(Col::has($vector, '2.2'));
        $this->assertFalse(Col::has($vector, '3.0.1'));
    }

    public function testInsertMap() {
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

        $map = Col::insert($map, 'one.two.string', 'bar');

        $this->assertEquals(Map {
            'number' => 123,
            'string' => 'foo',
            'boolean' => true,
            'one' => Map {
                'two' => Map {
                    'three' => Map {
                        'boolean' => false
                    },
                    'string' => 'bar'
                }
            }
        }, $map);
    }

    public function testInsertVector() {
        $this->markTestSkipped('todo - in Vec class');
    }

    public function testIsAlphaMap() {
        $map = Map {
            1 => 'foo',
            2 => 'bar',
            3 => '123'
        };

        $this->assertTrue(Col::isAlpha($map, false));
        $this->assertFalse(Col::isAlpha($map));
    }

    public function testIsAlphaVector() {
        $vector = Vector {
            'foo',
            'bar',
            '123'
        };

        $this->assertTrue(Col::isAlpha($vector, false));
        $this->assertFalse(Col::isAlpha($vector));
    }

    public function testIsNumericMap() {
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

    public function testIsNumericVectorp() {
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

    public function testKeyOfMap() {
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

    public function testKeyOfVector() {
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

    public function testPluckMap() {
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

    public function testPluckVector() {
        $map = Vector {
            Map {'name' => 'foo'},
            Map {'key' => 'value'},
            Map {'name' => 'bar'},
            Map {'key' => 'var'},
        };

        $this->assertEquals(Vector {'foo', 'bar'}, Col::pluck($map, 'name'));

        $map = Vector {
            Map {'meta' => Vector {1}},
            Map {'meta' => Vector {2}},
            Map {'meta' => Vector {3}},
        };

        $this->assertEquals(Vector {1, 2, 3}, Col::pluck($map, 'meta.0'));
    }

    public function testReduceMap() {
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

    public function testReduceVector() {
        $map = Vector {
            123,
            'foo',
            true,
            456.789
        };

        $this->assertEquals(Vector {
            123,
            456.789
        }, Col::reduce($map, Vector {0, 3}));
    }

    public function testRemoveMap() {
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

    public function testRemoveVector() {
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

        $vector = Col::remove($vector, '2.2.string');

        $this->assertEquals(Vector {
            123,
            'foo',
            Vector {
                456,
                'bar',
                Map {
                    'number' => 789
                }
            }
        }, $vector);

        $vector = Col::remove($vector, '2');

        $this->assertEquals(Vector {
            123,
            'foo'
        }, $vector);

        $vector = Col::remove($vector, '3');

        $this->assertEquals(Vector {
            123,
            'foo'
        }, $vector);
    }

    public function testSetMap() {
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

    public function testSetVector() {
        $this->markTestSkipped('todo - in Vec class');
    }

    public function testSomeMap() {
        $this->assertTrue(Col::some(Map {0 => 'foo', 1 => 123, 2 => true}, function($key, $value) {
            return is_numeric($value);
        }));

        $this->assertFalse(Col::some(Map {0 => 'foo', 1 => 123, 2 => true}, function($key, $value) {
            return is_float($value);
        }));
    }

    public function testSomeVector() {
        $this->assertTrue(Col::some(Vector {'foo', 123, true}, function($key, $value) {
            return is_numeric($value);
        }));

        $this->assertFalse(Col::some(Vector {'foo', 123, true}, function($key, $value) {
            return is_float($value);
        }));
    }

}