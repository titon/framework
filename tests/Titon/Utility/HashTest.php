<?php
namespace Titon\Utility;

use Titon\Test\TestCase;
use \Exception;
use \stdClass;

class HashTest extends TestCase {

    protected $expanded = array(
        'boolean' => true,
        'integer' => 123,
        'strings' => 'foobar',
        'numeric' => '1988',
        'empty' => array(),
        'one' => array(
            'depth' => 1,
            'two' => array(
                'depth' => 2,
                'three' => array(
                    'depth' => 3,
                    'false' => false,
                    'true' => true,
                    'null' => null,
                    'zero' => 0,
                    'four' => array(
                        'five' => array(
                            'six' => array(
                                'seven' => array(
                                    'key' => 'We can go deeper!'
                                )
                            )
                        )
                    )
                )
            )
        )
    );

    protected $collapsed = array(
        'boolean' => true,
        'integer' => 123,
        'strings' => 'foobar',
        'numeric' => '1988',
        'empty' => array(),
        'one.depth' => 1,
        'one.two.depth' => 2,
        'one.two.three.depth' => 3,
        'one.two.three.false' => false,
        'one.two.three.true' => true,
        'one.two.three.null' => null,
        'one.two.three.zero' => 0,
        'one.two.three.four.five.six.seven.key' => 'We can go deeper!'
    );

    public function testDepth() {
        $data = $this->expanded;

        $data1 = $data;
        $data2 = $data;
        $data3 = $data;
        unset($data1['one']['two']['three']['four'], $data2['one']['two'], $data3['one']['two']['three']);

        $data4 = new stdClass();
        $data4->integer = 123;
        $data4->one = new stdClass();
        $data4->one->foo = 'bar';

        $this->assertEquals(0, Hash::depth(array()));
        $this->assertEquals(8, Hash::depth($data));
        $this->assertEquals(4, Hash::depth($data1));
        $this->assertEquals(2, Hash::depth($data2));
        $this->assertEquals(3, Hash::depth($data3));
        $this->assertEquals(2, Hash::depth($data4));

        $this->assertEquals(0, Hash::depth(array(), true));
        $this->assertEquals(8, Hash::depth($data, true));
        $this->assertEquals(4, Hash::depth($data1, true));
        $this->assertEquals(2, Hash::depth($data2, true));
        $this->assertEquals(3, Hash::depth($data3, true));
        $this->assertEquals(2, Hash::depth($data4, true));

        foreach (array(true, false, null, 123, 'foo') as $type) {
            try {
                $this->assertEquals(0, Hash::depth($type));
                $this->assertTrue(false);
            } catch (Exception $e) {
                $this->assertTrue(true);
            }
        }
    }

    public function testEach() {
        $data = array(
            'integer' => 123,
            'number' => '456',
            'foo' => 'bar',
            'string' => 'test',
            'boolean' => true,
            'array' => array(
                525,
                'boo'
            )
        );

        $this->assertEquals(array(
            'integer' => 246,
            'number' => 912,
            'foo' => 'barwtf',
            'string' => 'testwtf',
            'boolean' => true,
            'array' => array(
                1050,
                'boowtf'
            )
        ), Hash::each($data, function($value, $key) {
            if (is_numeric($value)) {
                return $value * 2;
            } elseif (is_string($value)) {
                return $value . 'wtf';
            }

            return $value;
        }));

        $this->assertEquals(array(
            'integer' => 246,
            'number' => 912,
            'foo' => 'barwtf',
            'string' => 'testwtf',
            'boolean' => true,
            'array' => array(
                525,
                'boo'
            )
        ), Hash::each($data, function($value, $key) {
            if (is_numeric($value)) {
                return $value * 2;
            } elseif (is_string($value)) {
                return $value . 'wtf';
            }

            return $value;
        }, false));
    }

    public function testEvery() {
        $callback = function($value, $key) {
            return is_int($value);
        };

        $this->assertTrue(Hash::every(array( 123, 456 ), $callback));
        $this->assertFalse(Hash::every(array( 123, '456' ), $callback));
        $this->assertFalse(Hash::every(array( 123, '456', 'foo' ), $callback));
    }

    public function testExclude() {
        $this->assertEquals(array('foo' => 123), Hash::exclude(array('foo' => 123, 'bar' => 456, 'baz' => 789), array('bar', 'baz')));
    }

    public function testExpand() {
        $this->assertEquals($this->expanded, Hash::expand($this->collapsed));
    }

    public function testExtract() {
        $data = $this->expanded;

        foreach ($this->collapsed as $key => $value) {
            $this->assertEquals($value, Hash::extract($data, $key));
        }

        $this->assertEquals(null, Hash::extract(array(), 'some.path'));
        $this->assertEquals(null, Hash::extract($data, null));
        $this->assertEquals(null, Hash::extract($data, 'fake.path'));
        $this->assertEquals($data['one']['two']['three'], Hash::extract($data, 'one.two.three'));
        $this->assertEquals($data['one']['two']['three']['four']['five']['six'], Hash::extract($data, 'one.two.three.four.five.six'));
    }

    public function testFilter() {
        $data = $this->expanded;

        $match1 = $data;
        $match2 = $data;
        unset($match1['empty'], $match2['empty'], $match1['one']['two']['three']['false'], $match1['one']['two']['three']['null']);

        $this->assertEquals($match1, Hash::filter($data));
        $this->assertEquals($match2, Hash::filter($data, false));

        $data = array(
            'true' => true,
            'false' => false,
            'null' => null,
            'zero' => 0,
            'stringZero' => '0',
            'empty' => array(),
            'array' => array(
                'false' => false,
                'null' => null,
                'empty' => array()
            )
        );

        $this->assertEquals(array(
            'true' => true,
            'zero' => 0,
            'stringZero' => '0'
        ), Hash::filter($data));
    }

    public function testFlatten() {
        $match = $this->collapsed;
        $match['empty'] = null;

        $this->assertEquals($match, Hash::flatten($this->expanded));
    }

    public function testFlip() {
        $data = array(
            'true' => true,
            'false' => false,
            'null' => null,
            'zero' => 0,
            'stringZero' => '0',
            'empty' => array(),
            'array' => array(
                'false' => false,
                'null' => null,
                'empty' => array()
            )
        );

        $this->assertEquals(array(
            1 => 'true',
            0 => 'stringZero',
            'empty' => array(),
            'array' => array(
                'empty' => array()
            )
        ), Hash::flip($data));

        $data = array(
            'foo' => 'bar',
            1 => 'one',
            2 => 'two',
            true,
            false,
            null,
            'key' => 'value',
            'baz' => 'bar',
        );

        $this->assertEquals(array(
            'bar' => 'baz',
            'one' => '',
            'two' => '',
            1 => '',
            'value' => 'key'
        ), Hash::flip($data));

        $this->assertEquals(array(
            1 => 'boolean',
            123 => 'integer',
            'foobar' => 'strings',
            1988 => 'numeric',
            'empty' => array(),
            'one' => array(
                1 => 'depth',
                'two' => array(
                    2 => 'depth',
                    'three' => array(
                        3 => 'depth',
                        1 => 'true',
                        0 => 'zero',
                        'four' => array(
                            'five' => array(
                                'six' => array(
                                    'seven' => array(
                                        'We can go deeper!' => 'key'
                                    )
                                )
                            )
                        )
                    )
                )
            )
        ), Hash::flip($this->expanded));
    }

    public function testGet() {
        $data = $this->expanded;

        $this->assertEquals($data, Hash::get($data));
        $this->assertEquals(true, Hash::get($data, 'boolean'));
        $this->assertEquals($data['one']['two']['three'], Hash::get($data, 'one.two.three'));
        $this->assertEquals($data['one']['two']['three']['four']['five']['six'], Hash::get($data, 'one.two.three.four.five.six'));
    }

    public function testHas() {
        $data = $this->expanded;

        $this->assertTrue(Hash::has($data, 'boolean'));
        $this->assertTrue(Hash::has($data, 'empty'));
        $this->assertTrue(Hash::has($data, 'one.depth'));
        $this->assertTrue(Hash::has($data, 'one.two.depth'));
        $this->assertTrue(Hash::has($data, 'one.two.three.false'));
        $this->assertTrue(Hash::has($data, 'one.two.three.true'));
        $this->assertTrue(Hash::has($data, 'one.two.three.four.five.six.seven.key'));
        $this->assertTrue(Hash::has($data, 'one.two.three.null'));

        $this->assertFalse(Hash::has($data, null));
        $this->assertFalse(Hash::has(array(), 'foo'));
        $this->assertFalse(Hash::has($data, 'one.two.three.some.really.deep.depth'));
        $this->assertFalse(Hash::has($data, 'foo'));
        $this->assertFalse(Hash::has($data, 'foo.bar'));
        $this->assertFalse(Hash::has($data, 'empty.key'));
    }

    public function testInject() {
        $data = $this->expanded;
        $this->assertEquals($data, Hash::inject($data, 'one.depth', 2));

        $result = Hash::inject($data, 'one.foo', 'bar');
        $data['one']['foo'] = 'bar';
        $this->assertEquals($data, $result);
    }

    public function testInsert() {
        $data = array();

        foreach ($this->collapsed as $key => $value) {
            $data = Hash::insert($data, $key, $value);
        }

        $this->assertEquals($this->expanded, $data);
        $this->assertEquals(array(), Hash::insert(array(), '', 'value'));
    }

    public function testIsAlpha() {
        $this->assertTrue(Hash::isAlpha(array('foo', 'bar')));
        $this->assertTrue(Hash::isAlpha(array('foo' => 'bar', 'number' => '123'), false));
        $this->assertTrue(Hash::isAlpha(array('bar', '123'), false));

        $this->assertFalse(Hash::isAlpha(array('foo' => 'bar', 'number' => '123')));
        $this->assertFalse(Hash::isAlpha(array('bar', '123')));
        $this->assertFalse(Hash::isAlpha(array('foo' => 123)));
        $this->assertFalse(Hash::isAlpha(array(null)));
        $this->assertFalse(Hash::isAlpha(array(true)));
        $this->assertFalse(Hash::isAlpha(array(false)));
        $this->assertFalse(Hash::isAlpha(array(array())));
        $this->assertFalse(Hash::isAlpha(array(new stdClass())));
    }

    public function testIsNumeric() {
        $this->assertTrue(Hash::isNumeric(array('123', 456)));
        $this->assertTrue(Hash::isNumeric(array('foo' => 123, 'number' => '456')));

        $this->assertFalse(Hash::isNumeric(array('foo', 'bar')));
        $this->assertFalse(Hash::isNumeric(array('foo' => 'bar', 'number' => '123')));
        $this->assertFalse(Hash::isNumeric(array('bar', '123')));
        $this->assertFalse(Hash::isNumeric(array(null)));
        $this->assertFalse(Hash::isNumeric(array(true)));
        $this->assertFalse(Hash::isNumeric(array(false)));
        $this->assertFalse(Hash::isNumeric(array(array())));
        $this->assertFalse(Hash::isNumeric(array(new stdClass())));
    }

    public function testKeyOf() {
        $data = $this->expanded;

        $this->assertEquals(null, Hash::keyOf($data, 'fakeValue'));
        $this->assertEquals('boolean', Hash::keyOf($data, true));
        $this->assertEquals('one.two.three.depth', Hash::keyOf($data, 3));
    }

    public function testMap() {
        $data = array(
            'foo' => 'bar',
            'boolean' => true,
            'null' => null,
            'array' => array(),
            'number' => 123
        );

        $this->assertEquals(array(
            'foo' => 'BAR',
            'boolean' => true,
            'null' => null,
            'array' => array(),
            'number' => 123
        ), Hash::map($data, 'strtoupper'));

        $this->assertEquals(array(
            'foo' => 0,
            'boolean' => 1,
            'null' => 0,
            'array' => array(),
            'number' => 123
        ), Hash::map($data, 'intval'));

        $this->assertEquals(array(
            'foo' => 'string',
            'boolean' => 'true',
            'null' => 'null',
            'array' => array(),
            'number' => 'number'
        ), Hash::map($data, function($value) {
            if (is_numeric($value)) {
                return 'number';
            } elseif (is_bool($value)) {
                return $value ? 'true' : 'false';
            } elseif (is_null($value)) {
                return 'null';
            } elseif (is_string($value)) {
                return 'string';
            } else {
                return $value;
            }
        }));
    }

    public function testMatches() {
        $this->assertTrue(Hash::matches($this->expanded, $this->expanded));
        $this->assertTrue(Hash::matches(array(
            'foo' => 123,
            'bar' => 'baz',
            'array' => array()
        ), array(
            'foo' => 123,
            'bar' => 'baz',
            'array' => array()
        )));

        $this->assertFalse(Hash::matches($this->expanded, $this->collapsed));
        $this->assertFalse(Hash::matches(array(
            'foo' => '123',
            'bar' => 'baz',
            'array' => array()
        ), array(
            'foo' => 123,
            'bar' => 'baz',
            'array' => array()
        )));
    }

    public function testMerge() {
        $data1 = array(
            'foo' => 'bar',
            'boolean' => true,
            'string' => 'abc',
            'number' => 123,
            'one'
        );

        $data2 = array(
            'foo' => 'baz',
            'boolean' => false,
            'string' => 'xyz',
            'number' => 456,
            'two'
        );

        $this->assertEquals(array(
            'foo' => 'baz',
            'boolean' => false,
            'string' => 'xyz',
            'number' => 456,
            'two'
        ), Hash::merge($data1, $data2));

        $data1['array'] = array(
            'key' => 'value',
            123,
            true
        );

        $data2['array'] = array();

        $this->assertEquals(array(
            'foo' => 'baz',
            'boolean' => false,
            'string' => 'xyz',
            'number' => 456,
            'two',
            'array' => array(
                'key' => 'value',
                123,
                true
            )
        ), Hash::merge($data1, $data2));

        $data2['array'] = array(
            'key' => 'base',
            'foo' => 'bar',
            123
        );

        $this->assertEquals(array(
            'foo' => 'bar',
            'boolean' => true,
            'string' => 'abc',
            'number' => 123,
            'one',
            'array' => array(
                'key' => 'value',
                'foo' => 'bar',
                123,
                true
            )
        ), Hash::merge($data2, $data1));

        $this->assertEquals(array(), Hash::merge());
    }

    public function testOverwrite() {
        $data1 = array(
            'foo' => 'bar',
            123,
            'array' => array(
                'boolean' => true,
                'left' => 'left'
            )
        );

        $data2 = array(
            'foo' => 'baz',
            456,
            'array' => array(
                'boolean' => false,
                'right' => 'right'
            )
        );

        $this->assertEquals(array(
            'foo' => 'baz',
            456,
            'array' => array(
                'boolean' => false,
                'left' => 'left'
            )
        ), Hash::overwrite($data1, $data2));
    }

    public function testPluck() {
        $data = array(
            array( 'name' => 'Miles', 'user' => array( 'id' => 1 ) ),
            array( 'name' => 'Foo', 'user' => array( 'id' => 2) ),
            array( 'key' => 'value', 'user' => array( 'id' => 3 ) ),
            array( 'name' => 'Bar', 'user' => array( 'id' => 4 ) ),
            array( 'name' => 'Baz', 'user' => array( 'id' => 5 ) ),
        );

        $this->assertEquals(array('Miles', 'Foo', 'Bar', 'Baz'), Hash::pluck($data, 'name'));
        $this->assertEquals(array(1, 2, 3, 4, 5), Hash::pluck($data, 'user.id'));
    }

    public function testRange() {
        $this->assertEquals(array(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10), Hash::range(0, 10));
        $this->assertEquals(array(0 => 0, 2 => 2, 4 => 4, 6 => 6, 8 => 8, 10 => 10), Hash::range(0, 10, 2));
        $this->assertEquals(array(0 => 0, 3 => 3, 6 => 6, 9 => 9), Hash::range(0, 10, 3));
        $this->assertEquals(array(0 => 0, 13 => 13, 26 => 26, 39 => 39, 52 => 52, 65 => 65, 78 => 78, 91 => 91), Hash::range(0, 100, 13));
        $this->assertEquals(array(23 => 23, 29 => 29, 35 => 35, 41 => 41, 47 => 47, 53 => 53, 59 => 59, 65 => 65), Hash::range(23, 66, 6));

        $this->assertEquals(array(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10), Hash::range(0, 10, 1, false));
        $this->assertEquals(array(0, 2, 4, 6, 8, 10), Hash::range(0, 10, 2, false));
        $this->assertEquals(array(0, 3, 6, 9), Hash::range(0, 10, 3, false));
        $this->assertEquals(array(0, 13, 26, 39, 52, 65, 78, 91), Hash::range(0, 100, 13, false));
        $this->assertEquals(array(23, 29, 35, 41, 47, 53, 59, 65), Hash::range(23, 66, 6, false));

        // reverse
        $this->assertEquals(array(5 => 5, 4 => 4, 3 => 3, 2 => 2, 1 => 1, 0 => 0), Hash::range(5, 0, 1));
        $this->assertEquals(array(10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0), Hash::range(10, 0, 1, false));
        $this->assertEquals(array(65, 59, 53, 47, 41, 35, 29, 23), Hash::range(65, 23, 6, false));
    }

    public function testReduce() {
        $this->assertEquals(array('boolean' => true, 'integer' => 123), Hash::reduce($this->expanded, array('boolean', 'integer', 'foobar')));
    }

    public function testRemove() {
        $data = $this->expanded;
        $match = $data;

        unset($match['boolean']);
        $data = Hash::remove($data, 'boolean');
        $this->assertEquals($match, $data);

        unset($match['one']['depth']);
        $data = Hash::remove($data, 'one.depth');
        $this->assertEquals($match, $data);

        unset($match['one']['two']['depth']);
        $data = Hash::remove($data, 'one.two.depth');
        $this->assertEquals($match, $data);

        unset($match['one']['two']['three']['depth'], $match['one']['two']['three']['zero'], $match['one']['two']['three']['null']);
        $data = Hash::remove($data, 'one.two.three.depth');
        $data = Hash::remove($data, 'one.two.three.zero');
        $data = Hash::remove($data, 'one.two.three.null');
        $this->assertEquals($match, $data);

        unset($match['one']['two']['three']['four']['five']['six']['seven']['key']);
        $data = Hash::remove($data, 'one.two.three.four.five.six.seven.key');
        $this->assertEquals($match, $data);

        foreach (array(true, false, null, 123, 'foo') as $type) {
            $data = Hash::remove($data, $type);
            $this->assertEquals($match, $data);
        }

        $data = Hash::remove($data, 'a.fake.path');
        $this->assertEquals($match, $data);
    }

    public function testSet() {
        $data = $this->expanded;
        $match = $data;

        $data = Hash::set($data, 'key', 'value');
        $match['key'] = 'value';
        $this->assertEquals($match, $data);

        $data = Hash::set($data, 'key.key', 'value');
        $match['key'] = array('key' => 'value');
        $this->assertEquals($match, $data);

        $data = Hash::set($data, array(
            'key.key.key' => 'value',
            'true' => true,
            'one.false' => false
        ));
        $match['key']['key'] = array('key' => 'value');
        $match['true']= true;
        $match['one']['false'] = false;
        $this->assertEquals($match, $data);
    }

    public function testSome() {
        $this->assertTrue(Hash::some(array( 123, 'abc', true, null ), function($value, $key) {
            return is_string($value);
        }));

        $this->assertFalse(Hash::some(array( 123, true, null ), function($value, $key) {
            return is_string($value);
        }));
    }

}