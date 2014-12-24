<?hh
namespace Titon\Utility\State;

use Titon\Test\TestCase;

class GetTest extends TestCase {

    public function testClass() {
        $_GET = [
            'foo' => 123,
            'bar' => 'abc',
            'list' => [1, 2, 3],
            'map' => [
                'a' => 'A',
                'b' => 'B',
                'c' => 'C'
            ]
        ];

        Get::initialize($_GET);

        $this->assertEquals(Map {
            'foo' => 123,
            'bar' => 'abc',
            'list' => Vector {1, 2, 3},
            'map' => Map {
                'a' => 'A',
                'b' => 'B',
                'c' => 'C'
            }
        }, Get::all());

        $this->assertEquals(123, Get::get('foo'));
        $this->assertEquals('A', Get::get('map.a'));
        $this->assertEquals(Vector {1, 2, 3}, Get::get('list'));

        $this->assertTrue(Get::has('bar'));
        $this->assertTrue(Get::has('map.b'));

        $this->assertEquals(null, Get::get('baz'));
        $this->assertFalse(Get::has('map.d'));
    }

}