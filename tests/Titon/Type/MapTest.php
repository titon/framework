<?php
namespace Titon\Type;

use Titon\Test\TestCase;
use \Exception;

/**
 * @property \Titon\Type\Map $object
 * @property \Titon\Type\Map $books
 */
class MapTest extends TestCase {

    protected $map = [
        'integer' => 12345,
        'number' => '67890',
        'string' => 'Foobar',
        'boolean' => true,
        'null' => null,
        'zero' => 0,
        'empty' => '',
        'map' => ['foo' => 'bar'],
        'array' => ['foo', 'bar']
    ];

    protected $books = [];

    protected function setUp() {
        parent::setUp();

        $this->object = new Map($this->map);
        $this->books = new Map([
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
    }

    public function testAppend() {
        $this->object->append('append')->append(['append-array', 'append-array']);

        $this->assertEquals([
            'integer' => 12345,
            'number' => '67890',
            'string' => 'Foobar',
            'boolean' => true,
            'null' => null,
            'zero' => 0,
            'empty' => '',
            'map' => ['foo' => 'bar'],
            'array' => ['foo', 'bar'],
            'append',
            'append-array',
            'append-array'
        ], $this->object->value());
    }

    public function testChunk() {
        $this->assertEquals([
            [
                'integer' => 12345,
                'number' => '67890',
                'string' => 'Foobar',
            ], [
                'boolean' => true,
                'null' => null,
                'zero' => 0,
            ], [
                'empty' => '',
                'map' => ['foo' => 'bar'],
                'array' => ['foo', 'bar'],
            ]
        ], $this->object->chunk(3));
    }

    public function testClean() {
        $this->assertEquals([
            'integer' => 12345,
            'number' => '67890',
            'string' => 'Foobar',
            'boolean' => true,
            'zero' => 0,
            'map' => ['foo' => 'bar'],
            'array' => ['foo', 'bar']
        ], $this->object->clean()->value());
    }

    public function testCompare() {
        $compare = $this->object->compare(['Foobar'], ['strict' => false]);

        $this->assertEquals(new Map(['string' => 'Foobar']), $compare);
    }

    public function testCompareWithValueCallback() {
        $compare = $this->object->compare(['string' => 'FOOBAR', 'integer' => 53463], [
            'strict' => false,
            'valueCallback' => function ($k1, $k2) {
                return strcasecmp($k1, $k2);
            }
        ]);

        $this->assertEquals(new Map(['string' => 'Foobar']), $compare);
    }

    public function testCompareStrict() {
        $compare = $this->object->compare(['string' => 'Foobar', 'integer' => 67890], ['strict' => true]);

        $this->assertEquals(new Map(['string' => 'Foobar']), $compare);
    }

    public function testCompareStrictWithCallback() {
        $compare = $this->object->compare(['STRING' => 'Foobar', 'integer' => 67890], [
            'strict' => true,
            'callback' => null
        ]);

        $this->assertEquals(new Map([]), $compare);

        $compare = $this->object->compare(['STRING' => 'Foobar', 'integer' => 67890], [
            'strict' => true,
            'callback' => function ($k1, $k2) {
                return strcasecmp($k1, $k2);
            }
        ]);

        $this->assertEquals(new Map(['string' => 'Foobar']), $compare);
    }

    public function testCompareStrictWithCallbackAndValueCallback() {
        $compare = $this->object->compare(['STRING' => 'FOOBAR', 'integer' => 67890], [
            'strict' => true,
            'callback' => function ($k1, $k2) {
                return strcasecmp($k1, $k2);
            },
            'valueCallback' => function ($k1, $k2) {
                return strcasecmp($k1, $k2);
            }
        ]);

        $this->assertEquals(new Map(['string' => 'Foobar']), $compare);
    }

    public function testCompareStrictWithValueCallback() {
        $compare = $this->object->compare(['string' => 'FOOBAR', 'integer' => 67890], [
            'strict' => true,
            'valueCallback' => function ($k1, $k2) {
                return strcasecmp($k1, $k2);
            }
        ]);

        $this->assertEquals(new Map(['string' => 'Foobar']), $compare);
    }

    public function testCompareAgainstKeys() {
        $compare = $this->object->compare(['integer' => 67890, 'string' => 'Test'], ['on' => 'keys']);

        $this->assertEquals(new Map([
            'integer' => 12345,
            'string' => 'Foobar'
        ]), $compare);
    }

    public function testCompareAgainstKeysWithCallback() {
        $compare = $this->object->compare(['integer' => 67890, 'boolean' => false], [
            'on' => 'keys',
            'callback' => function($k1, $k2) {
                if ($k1 == $k2) {
                    return 0;
                } else if ($k1 > $k2) {
                    return 1;
                } else {
                    return -1;
                }
            }
        ]);

        $this->assertEquals(new Map([
            'integer' => 12345,
            'boolean' => true
        ]), $compare);
    }

    public function testConcat() {
        $map = $this->object->concat(['concat' => 'append']);

        $this->assertInstanceOf('\Titon\Type\Map', $map);
        $this->assertEquals([
            'integer' => 12345,
            'number' => '67890',
            'string' => 'Foobar',
            'boolean' => true,
            'null' => null,
            'zero' => 0,
            'empty' => '',
            'map' => ['foo' => 'bar'],
            'array' => ['foo', 'bar'],
            'concat' => 'append'
        ], $map->value());
    }

    public function testContains() {
        $this->assertTrue($this->object->contains(12345));
        $this->assertTrue($this->object->contains('Foobar'));
        $this->assertTrue($this->object->contains(null));
        $this->assertFalse($this->object->contains(63453435));
        $this->assertFalse($this->object->contains('Barbaz'));
        $this->assertFalse($this->object->contains(false));
    }

    public function testCount() {
        $this->assertEquals(9, $this->object->count());

        $this->object->flush();
        $this->assertEquals(0, $this->object->count());
    }

    public function testCountValues() {
        $this->object->append(['count', 'count']);

        $this->assertEquals([
            '12345' => 1,
            '67890' => 1,
            'Foobar' => 1,
            '0' => 1,
            '' => 1,
            'count' => 2
        ], $this->object->countValues());
    }

    public function testDepth() {
        $this->assertEquals(2, $this->object->depth());

        $this->object->set('deep.deep.deep.depth', 1);
        $this->assertEquals(4, $this->object->depth());
    }

    public function testDifference() {
        $diff = $this->object->difference([
            'integer' => 12345,
            'empty' => '',
            'string' => 'Foobar',
            'boolean' => true
        ], ['strict' => false]);

        $this->assertEquals(new Map([
            'number' => '67890',
            'zero' => 0
        ]), $diff);
    }

    public function testDifferenceWithValueCallback() {
        $diff = $this->object->difference([
            'integer' => 12345,
            'empty' => '',
            'string' => 'FOOBAR',
            'boolean' => true
        ], [
            'strict' => false,
            'valueCallback' => function ($k1, $k2) {
                return strcasecmp($k1, $k2);
            }
        ]);

        $this->assertEquals(new Map([
            'number' => '67890',
            'zero' => 0
        ]), $diff);
    }

    public function testDifferenceStrict() {
        $diff = $this->object->difference([
            'integer' => 12345,
            'empty' => '',
            'string' => 'Barbaz',
            'boolean' => true
        ], ['strict' => true]);

        $this->assertEquals(new Map([
            'number' => '67890',
            'string' => 'Foobar',
            'null' => null,
            'zero' => 0
        ]), $diff);
    }

    public function testDifferenceStrictWithCallback() {
        $diff = $this->object->difference([
            'integer' => 56789,
            'empty' => '',
            'STRING' => 'FOOBAR',
            'boolean' => true
        ], [
            'strict' => true
        ]);

        $this->assertEquals(new Map([
            'integer' => 12345,
            'number' => '67890',
            'string' => 'Foobar',
            'null' => null,
            'zero' => 0
        ]), $diff);

        $diff = $this->object->difference([
            'integer' => 12345,
            'empty' => '',
            'string' => 'Barbaz',
            'boolean' => true
        ], [
            'strict' => true,
            'callback' => function ($k1, $k2) {
                return strcasecmp($k1, $k2);
            }
        ]);

        $this->assertEquals(new Map([
            'number' => '67890',
            'string' => 'Foobar',
            'null' => null,
            'zero' => 0
        ]), $diff);
    }

    public function testDifferenceStrictWithCallbackAndValueCallback() {
        $diff = $this->object->difference([
            'INTEGER' => 12345,
            'fake' => '',
            'string' => 'FOOBAR',
            'BOOLEAN' => true
        ], [
            'strict' => true,
            'callback' => function ($k1, $k2) {
                return strcasecmp($k1, $k2);
            },
            'valueCallback' => function ($k1, $k2) {
                return strcasecmp($k1, $k2);
            }
        ]);

        $this->assertEquals(new Map([
            'number' => '67890',
            'zero' => 0,
            'null' => null,
            'empty' => ''
        ]), $diff);
    }

    public function testDifferenceStrictWithValueCallback() {
        $diff = $this->object->difference([
            'integer' => 12345,
            'fake' => '',
            'string' => 'FOOBAR',
            'boolean' => true
        ], [
            'strict' => true,
            'valueCallback' => function ($k1, $k2) {
                return strcasecmp($k1, $k2);
            }
        ]);

        $this->assertEquals(new Map([
            'number' => '67890',
            'zero' => 0,
            'null' => null,
            'empty' => ''
        ]), $diff);
    }

    public function testDifferenceAgainstKeys() {
        $diff = $this->object->difference([
            'integer' => 12345,
            'empty' => 'lies',
            'string' => 'Barbaz',
            'boolean' => false,
            'map' => ['key' => 'value']
        ], [
            'on' => 'keys'
        ]);

        $this->assertEquals(new Map([
            'number' => '67890',
            'null' => null,
            'zero' => 0
        ]), $diff);
    }

    public function testDifferenceAgainstKeysWithCallback() {
        $diff = $this->object->difference([
            'integer' => 12345,
            'empty' => 'lies',
            'string' => 'Barbaz',
            //'boolean' => false,
            'map' => ['key' => 'value']
        ], [
            'on' => 'keys',
            'callback' => function($k1, $k2) {
                if ($k1 == $k2) {
                    return 0;
                } else if ($k1 > $k2) {
                    return 1;
                } else {
                    return -1;
                }
            }
        ]);

        $this->assertEquals(new Map([
            'number' => '67890',
            'boolean' => true,
            'null' => null,
            'zero' => 0
        ]), $diff);
    }

    public function testEach() {
        $eached = $this->object->each(function($value, $key) {
            if (is_string($value)) {
                return $value . '---';
            }

            return $value;
        }, false);

        $this->assertEquals([
            'integer' => 12345,
            'number' => '67890---',
            'string' => 'Foobar---',
            'boolean' => true,
            'null' => null,
            'zero' => 0,
            'empty' => '---',
            'map' => ['foo' => 'bar'],
            'array' => ['foo', 'bar']
        ], $eached->value());

        // recursive
        $eached = $eached->each(function($value, $key) {
            if (is_string($value)) {
                return $value . '+++';
            }

            return $value;
        }, true);

        $this->assertEquals([
            'integer' => 12345,
            'number' => '67890---+++',
            'string' => 'Foobar---+++',
            'boolean' => true,
            'null' => null,
            'zero' => 0,
            'empty' => '---+++',
            'map' => ['foo' => 'bar+++'],
            'array' => ['foo+++', 'bar+++']
        ], $eached->value());
    }

    public function testEquals() {
        $this->assertFalse($this->object->equals([]));
        $this->assertTrue($this->object->equals($this->map));
    }

    public function testErase() {
        $object = $this->object->erase(null)->erase('Foobar');

        $this->assertEquals([
            'integer' => 12345,
            'number' => '67890',
            'boolean' => true,
            'zero' => 0,
            'empty' => '',
            'map' => ['foo' => 'bar'],
            'array' => ['foo', 'bar']
        ], $object->value());
    }

    public function testEvery() {
        $this->assertTrue($this->object->every(function($value, $key) {
            return true;
        }));

        $this->assertFalse($this->object->every(function($value, $key) {
            return is_numeric($value);
        }));
    }

    public function testExtract() {
        $this->assertEquals(12345, $this->object->extract('integer'));
        $this->assertEquals(0, $this->object->extract('zero'));
        $this->assertEquals(null, $this->object->extract('null'));
        $this->assertEquals('bar', $this->object->extract('map.foo'));
    }

    public function testFilter() {
        $this->assertEquals(new Map([
            'integer' => 12345,
            'number' => '67890',
            'string' => 'Foobar',
            'boolean' => true,
            'zero' => 0,
            'map' => ['foo' => 'bar'],
            'array' => ['foo', 'bar']
        ]), $this->object->filter());
    }

    public function testFilterCallback() {
        $this->assertEquals(new Map([
            'number' => '67890',
            'string' => 'Foobar',
            'empty' => ''
        ]), $this->object->filter(function($value) {
            return is_string($value);
        }));
    }

    public function testFind() {
        $this->assertEquals([
            'id' => 3,
            'series_id' => 1,
            'name' => 'A Storm of Swords',
            'isbn' => '0-553-10663-5',
            'released' => '2000-11-11'
        ], $this->books->find(3));
    }

    public function testFindByKey() {
        $this->assertEquals([
            'id' => 3,
            'series_id' => 1,
            'name' => 'A Storm of Swords',
            'isbn' => '0-553-10663-5',
            'released' => '2000-11-11'
        ], $this->books->find('0-553-10663-5', 'isbn'));
    }

    public function testFindByCallback() {
        $this->assertEquals([
            'id' => 2,
            'series_id' => 1,
            'name' => 'A Clash of Kings',
            'isbn' => '0-553-10803-4',
            'released' => '1999-02-25'
        ], $this->books->find(function($value) {
            return (strpos($value['name'], 'King') !== false);
        }));
    }

    public function testFindNoResult() {
        $this->assertEquals(null, $this->books->find(666));
    }

    public function testFindMany() {
        $this->assertEquals(new Map([
            [
                'id' => 3,
                'series_id' => 1,
                'name' => 'A Storm of Swords',
                'isbn' => '0-553-10663-5',
                'released' => '2000-11-11'
            ],
            [
                'id' => 10,
                'series_id' => 2,
                'name' => 'Harry Potter and the Order of the Phoenix',
                'isbn' => '0-7475-5100-6',
                'released' => '2003-06-21'
            ]
        ]), $this->books->findMany([3, 10]));
    }

    public function testFindManyByKey() {
        $this->assertEquals(new Map([
            ['id' => 13, 'series_id' => 3, 'name' => 'The Fellowship of the Ring', 'isbn' => '', 'released' => '1954-07-24'],
            ['id' => 14, 'series_id' => 3, 'name' => 'The Two Towers', 'isbn' => '', 'released' => '1954-11-11'],
            ['id' => 15, 'series_id' => 3, 'name' => 'The Return of the King', 'isbn' => '', 'released' => '1955-10-25'],
        ]), $this->books->findMany(3, 'series_id'));
    }

    public function testFindManyByCallback() {
        $this->assertEquals(new Map([
            ['id' => 2, 'series_id' => 1, 'name' => 'A Clash of Kings', 'isbn' => '0-553-10803-4', 'released' => '1999-02-25'],
            ['id' => 15, 'series_id' => 3, 'name' => 'The Return of the King', 'isbn' => '', 'released' => '1955-10-25'],
        ]), $this->books->findMany(function($value) {
            return (strpos($value['name'], 'King') !== false);
        }));
    }

    public function testFindManyNoResult() {
        $this->assertEquals(new Map(), $this->books->findMany(666));
    }

    public function testFirst() {
        $this->object->append('last')->prepend('first');

        $this->assertEquals('first', $this->object->first());
        $this->assertEquals([
            'first',
            'integer' => 12345,
            'number' => '67890',
            'string' => 'Foobar',
            'boolean' => true,
            'null' => null,
            'zero' => 0,
            'empty' => '',
            'map' => ['foo' => 'bar'],
            'array' => ['foo', 'bar'],
            'last'
        ], $this->object->value());
    }

    public function testFlatten() {
        $this->assertEquals(new Map([
            'integer' => 12345,
            'number' => '67890',
            'string' => 'Foobar',
            'boolean' => true,
            'null' => null,
            'zero' => 0,
            'empty' => '',
            'map.foo' => 'bar',
            'array.0' => 'foo',
            'array.1' => 'bar'
        ]), $this->object->flatten());
    }

    public function testFlip() {
        $this->assertEquals(new Map([
            '12345' => 'integer',
            '67890' => 'number',
            'Foobar' => 'string',
            1 => 'boolean',
            0 => 'zero',
            'map' => ['bar' => 'foo'],
            'array' => ['foo' => '', 'bar' => '']
        ]), $this->object->flip());
    }

    public function testFlush() {
        $this->object->flush();

        $this->assertEquals([], $this->object->value());
    }

    public function testGet() {
        $this->assertEquals(12345, $this->object->get('integer'));
        $this->assertEquals('Foobar', $this->object->get('string'));
        $this->assertEquals(null, $this->object->get('null'));
        $this->assertEquals(null, $this->object->get('fakeKey'));
    }

    public function testGroupBy() {
        $this->assertEquals([
            1 => new Map([
                ['id' => 1, 'series_id' => 1, 'name' => 'A Game of Thrones', 'isbn' => '0-553-10354-7', 'released' => '1996-08-02'],
                ['id' => 2, 'series_id' => 1, 'name' => 'A Clash of Kings', 'isbn' => '0-553-10803-4', 'released' => '1999-02-25'],
                ['id' => 3, 'series_id' => 1, 'name' => 'A Storm of Swords', 'isbn' => '0-553-10663-5', 'released' => '2000-11-11'],
                ['id' => 4, 'series_id' => 1, 'name' => 'A Feast for Crows', 'isbn' => '0-553-80150-3', 'released' => '2005-11-02'],
                ['id' => 5, 'series_id' => 1, 'name' => 'A Dance with Dragons', 'isbn' => '0-553-80147-3', 'released' => '2011-07-19'],
            ]),
            2 => new Map([
                ['id' => 6, 'series_id' => 2, 'name' => 'Harry Potter and the Philosopher\'s Stone', 'isbn' => '0-7475-3269-9', 'released' => '1997-06-27'],
                ['id' => 7, 'series_id' => 2, 'name' => 'Harry Potter and the Chamber of Secrets', 'isbn' => '0-7475-3849-2', 'released' => '1998-07-02'],
                ['id' => 8, 'series_id' => 2, 'name' => 'Harry Potter and the Prisoner of Azkaban', 'isbn' => '0-7475-4215-5', 'released' => '1999-07-09'],
                ['id' => 9, 'series_id' => 2, 'name' => 'Harry Potter and the Goblet of Fire', 'isbn' => '0-7475-4624-X', 'released' => '2000-07-08'],
                ['id' => 10, 'series_id' => 2, 'name' => 'Harry Potter and the Order of the Phoenix', 'isbn' => '0-7475-5100-6', 'released' => '2003-06-21'],
                ['id' => 11, 'series_id' => 2, 'name' => 'Harry Potter and the Half-blood Prince', 'isbn' => '0-7475-8108-8', 'released' => '2005-07-16'],
                ['id' => 12, 'series_id' => 2, 'name' => 'Harry Potter and the Deathly Hallows', 'isbn' => '0-545-01022-5', 'released' => '2007-07-21'],
            ]),
            3 => new Map([
                ['id' => 13, 'series_id' => 3, 'name' => 'The Fellowship of the Ring', 'isbn' => '', 'released' => '1954-07-24'],
                ['id' => 14, 'series_id' => 3, 'name' => 'The Two Towers', 'isbn' => '', 'released' => '1954-11-11'],
                ['id' => 15, 'series_id' => 3, 'name' => 'The Return of the King', 'isbn' => '', 'released' => '1955-10-25'],
            ])
        ], $this->books->groupBy('series_id'));
    }

    public function testHas() {
        $this->assertTrue($this->object->has('integer'));
        $this->assertTrue($this->object->has('string'));
        $this->assertTrue($this->object->has('map.foo'));
        $this->assertFalse($this->object->has('fakeKey'));
    }

    public function testIndexOf() {
        $this->assertEquals(0, $this->object->indexOf('integer'));
        $this->assertEquals(2, $this->object->indexOf('string'));
        $this->assertEquals(7, $this->object->indexOf('map'));
        $this->assertEquals(-1, $this->object->indexOf('fakeKey'));
    }

    public function testIsEmpty() {
        $this->assertFalse($this->object->isEmpty());

        $this->object->flush();
        $this->assertTrue($this->object->isEmpty());
    }

    public function testIsNotEmpty() {
        $this->assertTrue($this->object->isNotEmpty());

        $this->object->flush();
        $this->assertFalse($this->object->isNotEmpty());
    }

    public function testKeys() {
        $this->assertEquals(['integer', 'number', 'string', 'boolean', 'null', 'zero', 'empty', 'map', 'array'], $this->object->keys());
    }

    public function testLast() {
        $this->object->append('last')->prepend('first');

        $this->assertEquals('last', $this->object->last());
        $this->assertEquals([
            'first',
            'integer' => 12345,
            'number' => '67890',
            'string' => 'Foobar',
            'boolean' => true,
            'null' => null,
            'zero' => 0,
            'empty' => '',
            'map' => ['foo' => 'bar'],
            'array' => ['foo', 'bar'],
            'last'
        ], $this->object->value());
    }

    public function testLength() {
        $this->assertEquals(9, $this->object->length());

        $this->object->append([1, 2]);
        $this->assertEquals(11, $this->object->length());
    }

    public function testMap() {
        $this->assertEquals(new Map([
            'integer' => 12345,
            'number' => 67890,
            'string' => 1,
            'boolean' => 1,
            'null' => 1,
            'zero' => 0,
            'empty' => 1,
            'map' => 1,
            'array' => 1
        ]), $this->object->map(function($value) {
            if (!is_numeric($value)) {
                return 1;
            }

            return (int) $value;
        }));
    }

    public function testMerge() {
        $this->assertEquals(new Map([
            'integer' => 12345,
            'number' => '67890',
            'string' => 'Barbaz',
            'boolean' => false,
            'null' => null,
            'zero' => 1,
            'empty' => '',
            'map' => ['foo' => 'baz'],
            'array' => ['foo', 'bar'],
            'new' => 'key'
        ]), $this->object->merge([
            'string' => 'Barbaz',
            'zero' => 1,
            'boolean' => false,
            'new' => 'key',
            'map' => ['foo' => 'baz']
        ]));
    }

    public function testPrepend() {
        $this->object->prepend('prepend')->prepend(['prepend-array', 'prepend-array']);

        $this->assertEquals([
            'prepend-array',
            'prepend-array',
            'prepend',
            'integer' => 12345,
            'number' => '67890',
            'string' => 'Foobar',
            'boolean' => true,
            'null' => null,
            'zero' => 0,
            'empty' => '',
            'map' => ['foo' => 'bar'],
            'array' => ['foo', 'bar']
        ], $this->object->value());
    }

    public function testProduct() {
        $this->assertEquals(0, $this->object->product());
    }

    public function testPluck() {
        $this->assertEquals(range(1, 15), $this->books->pluck());
    }

    public function testPluckByKey() {
        $this->assertEquals([
            '0-553-10354-7',
            '0-553-10803-4',
            '0-553-10663-5',
            '0-553-80150-3',
            '0-553-80147-3',
            '0-7475-3269-9',
            '0-7475-3849-2',
            '0-7475-4215-5',
            '0-7475-4624-X',
            '0-7475-5100-6',
            '0-7475-8108-8',
            '0-545-01022-5',
            '',
            '',
            ''
        ], $this->books->pluck('isbn'));
    }

    public function testRandom() {
        $this->assertNotEquals('random', $this->object->random());
    }

    public function testReduce() {
        $this->assertEquals(80235, $this->object->reduce(function($result, $value) {
            if (is_numeric($value)) {
                return $result + $value;
            }

            return $result;
        }));
    }

    public function testRemove() {
        $this->object->remove('null')->remove('map.foo');

        $this->assertEquals([
            'integer' => 12345,
            'number' => '67890',
            'string' => 'Foobar',
            'boolean' => true,
            'zero' => 0,
            'empty' => '',
            'map' => [],
            'array' => ['foo', 'bar']
        ], $this->object->value());
    }

    public function testReverse() {
        $this->object->reverse();

        $this->assertEquals([
            'array' => ['foo', 'bar'],
            'map' => ['foo' => 'bar'],
            'empty' => '',
            'zero' => 0,
            'null' => null,
            'boolean' => true,
            'string' => 'Foobar',
            'number' => '67890',
            'integer' => 12345
        ], $this->object->value());
    }

    public function testShuffle() {
        $this->assertNotEquals($this->map, $this->object->shuffle()->value());
    }

    public function testSlice() {
        $this->assertEquals(new Map(['integer' => 12345]), $this->object->slice(0, 1));

        $this->assertEquals(new Map([
            'zero' => 0,
            'empty' => '',
            'map' => ['foo' => 'bar']
        ]), $this->object->slice(5, 3));

        $this->assertEquals(new Map([
            'map' => ['foo' => 'bar'],
            'array' => ['foo', 'bar']
        ]), $this->object->slice(7));
    }

    public function testSome() {
        $this->assertTrue($this->object->some(function($value, $key) {
            return ($value === true);
        }));

        $this->assertTrue($this->object->some(function($value, $key) {
            return ($value !== false);
        }));
    }

    public function testSort() {
        $map = new Map([
            'a' => 5,
            'b' => 3,
            'c' => 4,
            'd' => 1,
            'e' => 2
        ]);

        // by value
        $map = $map->sort();

        $this->assertEquals([
            'd' => 1,
            'e' => 2,
            'b' => 3,
            'c' => 4,
            'a' => 5
        ], $map->value());

        // reverse it
        $map = $map->sort(true);

        $this->assertEquals([
            'a' => 5,
            'c' => 4,
            'b' => 3,
            'e' => 2,
            'd' => 1,
        ], $map->value());

        // by value with no preserving
        $map = $map->sort(['preserve' => false]);

        $this->assertEquals([1, 2, 3, 4, 5], $map->value());

        // reverse it
        $map = $map->sort(['preserve' => false, 'reverse' => true]);

        $this->assertEquals([5, 4, 3, 2, 1], $map->value());
    }

    public function testSortWithCallback() {
        $map = new Map([
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
                return 1;
            } else {
                return -1;
            }
        };

        // by value with callback
        $map = $map->sort($callback);

        $this->assertEquals([
            'd' => 1,
            'e' => 2,
            'b' => 3,
            'c' => 4,
            'a' => 5
        ], $map->value());

        // reverse it
        $map = $map->sort(true);

        $this->assertEquals([
            'a' => 5,
            'c' => 4,
            'b' => 3,
            'e' => 2,
            'd' => 1,
        ], $map->value());

        // by value with no preserving
        $map = $map->sort(['preserve' => false, 'callback' => $callback]);

        $this->assertEquals([1, 2, 3, 4, 5], $map->value());

        // reverse it
        $map = $map->sort(['preserve' => false, 'reverse' => true]);

        $this->assertEquals([5, 4, 3, 2, 1], $map->value());
    }

    public function testSortKeys() {
        $map = new Map([
            'a' => 5,
            'b' => 3,
            'c' => 4,
            'd' => 1,
            'e' => 2
        ]);

        // by key
        $map->sort(['on' => 'keys']);

        $this->assertEquals([
            'a' => 5,
            'b' => 3,
            'c' => 4,
            'd' => 1,
            'e' => 2,
        ], $map->value());

        // reverse it
        $map->sort(true);

        $this->assertEquals([
            'd' => 1,
            'e' => 2,
            'c' => 4,
            'b' => 3,
            'a' => 5,
        ], $map->value());
    }

    public function testSortKeysWithCallback() {
        $map = new Map([
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
                return 1;
            } else {
                return -1;
            }
        };

        // by key
        $map->sort(['on' => 'keys', 'callback' => $callback]);

        $this->assertEquals([
            'a' => 5,
            'b' => 3,
            'c' => 4,
            'd' => 1,
            'e' => 2,
        ], $map->value());

        // reverse it
        $map->sort(['on' => 'keys', 'reverse' => true, 'callback' => $callback]);

        $this->assertEquals([
            'd' => 1,
            'e' => 2,
            'c' => 4,
            'b' => 3,
            'a' => 5,
        ], $map->value());
    }

    public function testSortBy() {
        $this->assertEquals(new Map([
            ['id' => 2, 'series_id' => 1, 'name' => 'A Clash of Kings', 'isbn' => '0-553-10803-4', 'released' => '1999-02-25'],
            ['id' => 5, 'series_id' => 1, 'name' => 'A Dance with Dragons', 'isbn' => '0-553-80147-3', 'released' => '2011-07-19'],
            ['id' => 4, 'series_id' => 1, 'name' => 'A Feast for Crows', 'isbn' => '0-553-80150-3', 'released' => '2005-11-02'],
            ['id' => 1, 'series_id' => 1, 'name' => 'A Game of Thrones', 'isbn' => '0-553-10354-7', 'released' => '1996-08-02'],
            ['id' => 3, 'series_id' => 1, 'name' => 'A Storm of Swords', 'isbn' => '0-553-10663-5', 'released' => '2000-11-11'],
            ['id' => 7, 'series_id' => 2, 'name' => 'Harry Potter and the Chamber of Secrets', 'isbn' => '0-7475-3849-2', 'released' => '1998-07-02'],
            ['id' => 12, 'series_id' => 2, 'name' => 'Harry Potter and the Deathly Hallows', 'isbn' => '0-545-01022-5', 'released' => '2007-07-21'],
            ['id' => 9, 'series_id' => 2, 'name' => 'Harry Potter and the Goblet of Fire', 'isbn' => '0-7475-4624-X', 'released' => '2000-07-08'],
            ['id' => 11, 'series_id' => 2, 'name' => 'Harry Potter and the Half-blood Prince', 'isbn' => '0-7475-8108-8', 'released' => '2005-07-16'],
            ['id' => 10, 'series_id' => 2, 'name' => 'Harry Potter and the Order of the Phoenix', 'isbn' => '0-7475-5100-6', 'released' => '2003-06-21'],
            ['id' => 6, 'series_id' => 2, 'name' => 'Harry Potter and the Philosopher\'s Stone', 'isbn' => '0-7475-3269-9', 'released' => '1997-06-27'],
            ['id' => 8, 'series_id' => 2, 'name' => 'Harry Potter and the Prisoner of Azkaban', 'isbn' => '0-7475-4215-5', 'released' => '1999-07-09'],
            ['id' => 13, 'series_id' => 3, 'name' => 'The Fellowship of the Ring', 'isbn' => '', 'released' => '1954-07-24'],
            ['id' => 15, 'series_id' => 3, 'name' => 'The Return of the King', 'isbn' => '', 'released' => '1955-10-25'],
            ['id' => 14, 'series_id' => 3, 'name' => 'The Two Towers', 'isbn' => '', 'released' => '1954-11-11'],
        ]), $this->books->sortBy('name'));
    }

    public function testSortByCustomComparator() {
        $this->assertEquals(new Map([
            ['id' => 5, 'series_id' => 1, 'name' => 'A Dance with Dragons', 'isbn' => '0-553-80147-3', 'released' => '2011-07-19'],
            ['id' => 12, 'series_id' => 2, 'name' => 'Harry Potter and the Deathly Hallows', 'isbn' => '0-545-01022-5', 'released' => '2007-07-21'],
            ['id' => 4, 'series_id' => 1, 'name' => 'A Feast for Crows', 'isbn' => '0-553-80150-3', 'released' => '2005-11-02'],
            ['id' => 11, 'series_id' => 2, 'name' => 'Harry Potter and the Half-blood Prince', 'isbn' => '0-7475-8108-8', 'released' => '2005-07-16'],
            ['id' => 10, 'series_id' => 2, 'name' => 'Harry Potter and the Order of the Phoenix', 'isbn' => '0-7475-5100-6', 'released' => '2003-06-21'],
            ['id' => 3, 'series_id' => 1, 'name' => 'A Storm of Swords', 'isbn' => '0-553-10663-5', 'released' => '2000-11-11'],
            ['id' => 9, 'series_id' => 2, 'name' => 'Harry Potter and the Goblet of Fire', 'isbn' => '0-7475-4624-X', 'released' => '2000-07-08'],
            ['id' => 8, 'series_id' => 2, 'name' => 'Harry Potter and the Prisoner of Azkaban', 'isbn' => '0-7475-4215-5', 'released' => '1999-07-09'],
            ['id' => 2, 'series_id' => 1, 'name' => 'A Clash of Kings', 'isbn' => '0-553-10803-4', 'released' => '1999-02-25'],
            ['id' => 7, 'series_id' => 2, 'name' => 'Harry Potter and the Chamber of Secrets', 'isbn' => '0-7475-3849-2', 'released' => '1998-07-02'],
            ['id' => 6, 'series_id' => 2, 'name' => 'Harry Potter and the Philosopher\'s Stone', 'isbn' => '0-7475-3269-9', 'released' => '1997-06-27'],
            ['id' => 1, 'series_id' => 1, 'name' => 'A Game of Thrones', 'isbn' => '0-553-10354-7', 'released' => '1996-08-02'],
            ['id' => 15, 'series_id' => 3, 'name' => 'The Return of the King', 'isbn' => '', 'released' => '1955-10-25'],
            ['id' => 14, 'series_id' => 3, 'name' => 'The Two Towers', 'isbn' => '', 'released' => '1954-11-11'],
            ['id' => 13, 'series_id' => 3, 'name' => 'The Fellowship of the Ring', 'isbn' => '', 'released' => '1954-07-24'],
        ]), $this->books->sortBy(function($a, $b) {
            return strtotime($a['released']) - strtotime($b['released']);
        }, SORT_REGULAR, true));
    }

    public function testSortNatural() {
        $map = new Map([
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
        $map->sortNatural();

        $this->assertEquals([
            1 => 'apple',
            7 => 'banana',
            2 => 'item 1',
            3 => 'item 5',
            5 => 'ITEM 10',
            6 => 'ITEM 55',
            0 => 'item 109',
            4 => 'orange'
        ], $map->value());

        // case-sensitive
        $map->sortNatural(true);

        $this->assertEquals([
            1 => 'apple',
            7 => 'banana',
            5 => 'ITEM 10',
            6 => 'ITEM 55',
            2 => 'item 1',
            3 => 'item 5',
            0 => 'item 109',
            4 => 'orange'
        ], $map->value());
    }

    public function testSplice() {
        $this->assertEquals(new Map([
            'integer' => 12345,
            'spliced' => true,
            'empty' => '',
            'map' => ['foo' => 'bar'],
            'array' => ['foo', 'bar']
        ]), $this->object->splice(1, 5, ['spliced' => true]));
    }

    public function testSum() {
        $this->assertEquals(80236, $this->object->sum());
    }

    public function testJsonSerialize() {
        $this->assertEquals($this->map, $this->object->jsonSerialize());
    }

    public function testTo() {
        $xml = '<?xml version="1.0" encoding="utf-8"?>' . "\n" . '<collection><integer>12345</integer><number>67890</number><string>Foobar</string><boolean>true</boolean><null></null><zero>0</zero><empty></empty><map><foo>bar</foo></map><array>foo</array><array>bar</array></collection>';

        $this->assertEquals($this->map, $this->object->to('array'));
        $this->assertEquals($this->map, $this->object->toArray());
        $this->assertEquals(json_encode($this->map), $this->object->toJson());
        $this->assertEquals($xml, $this->object->toXml());
        $this->assertEquals(serialize($this->map), $this->object->serialize());

        try {
            $this->object->to('foobar');
            $this->assertTrue(true);
        } catch (Exception $e) {
            $this->assertTrue(true);
        }
    }

    public function testToCallback() {
        $this->assertEquals([
            'integer' => [12345],
            'number' => ['67890'],
            'string' => ['Foobar'],
            'boolean' => [true],
            'null' => [],
            'zero' => [0],
            'empty' => [''],
            'map' => ['foo' => 'bar'],
            'array' => ['foo', 'bar']
        ], $this->object->to(function($value) {
            return (array) $value;
        }));
    }

    public function testToMethods() {
        $xml = '<?xml version="1.0" encoding="utf-8"?>' . "\n" . '<collection><integer>12345</integer><number>67890</number><string>Foobar</string><boolean>true</boolean><null></null><zero>0</zero><empty></empty><map><foo>bar</foo></map><array>foo</array><array>bar</array></collection>';

        $this->assertEquals($this->map, $this->object->toArray());
        $this->assertEquals(json_encode($this->map), $this->object->toJson());
        $this->assertEquals($xml, $this->object->toXml());
        $this->assertEquals(serialize($this->map), $this->object->serialize());
    }

    public function testToString() {
        $serialized = 'a:9:{s:7:"integer";i:12345;s:6:"number";s:5:"67890";s:6:"string";s:6:"Foobar";s:7:"boolean";b:1;s:4:"null";N;s:4:"zero";i:0;s:5:"empty";s:0:"";s:3:"map";a:1:{s:3:"foo";s:3:"bar";}s:5:"array";a:2:{i:0;s:3:"foo";i:1;s:3:"bar";}}';

        $this->assertEquals($serialized, $this->object->toString());
        $this->assertEquals($serialized, (string) $this->object);
    }

    public function testUnique() {
        $this->assertEquals(new Map([
            'integer' => 12345,
            'number' => '67890',
            'string' => 'Foobar',
            'null' => null,
            'map' => ['foo' => 'bar'],
            'array' => ['foo', 'bar']
        ]), $this->object->unique());
    }

    public function testValues() {
        $this->assertEquals([
            12345,
            '67890',
            'Foobar',
            true,
            null,
            0,
            '',
            ['foo' => 'bar'],
            ['foo', 'bar']
        ], $this->object->values());
    }

    public function testSet() {
        $this->object->set('key', 'value')->set('map.foo', 'overwritten');

        $this->assertEquals([
            'integer' => 12345,
            'number' => '67890',
            'string' => 'Foobar',
            'boolean' => true,
            'null' => null,
            'zero' => 0,
            'empty' => '',
            'map' => ['foo' => 'overwritten'],
            'array' => ['foo', 'bar'],
            'key' => 'value'
        ], $this->object->value());
    }

    public function testArrayAccess() {
        $this->assertTrue(isset($this->object['integer']));
        $this->assertTrue(isset($this->object['map']));
        $this->assertFalse(isset($this->object['fakeKey']));

        $this->assertEquals(true, $this->object['boolean']);
        $this->assertEquals('', $this->object['empty']);
        $this->assertEquals('bar', $this->object['map']['foo']);
        $this->assertEquals(null, $this->object['fakeKey']);

        $this->object[] = 'no-key';
        $this->object['key'] = 'value';

        $this->assertEquals([
            'integer' => 12345,
            'number' => '67890',
            'string' => 'Foobar',
            'boolean' => true,
            'null' => null,
            'zero' => 0,
            'empty' => '',
            'map' => ['foo' => 'bar'],
            'array' => ['foo', 'bar'],
            0 => 'no-key',
            'key' => 'value'
        ], $this->object->value());

        unset($this->object['number'], $this->object['string']);

        $this->assertEquals([
            'integer' => 12345,
            'boolean' => true,
            'null' => null,
            'zero' => 0,
            'empty' => '',
            'map' => ['foo' => 'bar'],
            'array' => ['foo', 'bar'],
            0 => 'no-key',
            'key' => 'value'
        ], $this->object->value());
    }

    public function testIterator() {
        $values = [];

        foreach ($this->object as $value) {
            $values[] = $value;
        }

        $this->assertEquals([
            12345,
            '67890',
            'Foobar',
            true,
            null,
            0,
            '',
            ['foo' => 'bar'],
            ['foo', 'bar']
        ], $this->object->values());
    }

    public function testMethodChaining() {
        $map = new Map();
        $map = $map->append('append')->prepend('prepend')->set('set.nested', 'value')->append([1, 2, 3, 4, 5]);

        $this->assertEquals([
            'prepend',
            'append',
            'set' => ['nested' => 'value'],
            1,
            2,
            3,
            4,
            5
        ], $map->value());

        $map = $map->prepend(['foo', 'bar'])->set('zero', '0')->set('null', null)->set('boolean', false)->each(function($value, $key) {
            if (is_numeric($value)) {
                return $value * 10;
            }

            return $value;
        });

        $this->assertEquals([
            'bar',
            'foo',
            'prepend',
            'append',
            'set' => ['nested' => 'value'],
            10,
            20,
            30,
            40,
            50,
            'zero' => 0,
            'null' => null,
            'boolean' => false
        ], $map->value());

        $map = $map->filter()->reverse(false);

        $this->assertEquals([
            'zero' => 0,
            50,
            40,
            30,
            20,
            10,
            'set' => ['nested' => 'value'],
            'append',
            'prepend',
            'foo',
            'bar'
        ], $map->value());
    }

}