<?hh
namespace Titon\Utility\State;

use Titon\Test\TestCase;

class SessionTest extends TestCase {

    public function testClass() {
        $_SESSION = [
            'foo' => 123,
            'bar' => 'abc',
            'list' => [1, 2, 3],
            'map' => [
                'a' => 'A',
                'b' => 'B',
                'c' => 'C'
            ]
        ];

        Session::initialize($_SESSION);

        $this->assertEquals(Map {
            'foo' => 123,
            'bar' => 'abc',
            'list' => Vector {1, 2, 3},
            'map' => Map {
                'a' => 'A',
                'b' => 'B',
                'c' => 'C'
            }
        }, Session::all());

        $this->assertEquals(123, Session::get('foo'));
        $this->assertEquals('A', Session::get('map.a'));
        $this->assertEquals(Vector {1, 2, 3}, Session::get('list'));

        $this->assertTrue(Session::has('bar'));
        $this->assertTrue(Session::has('map.b'));

        $this->assertEquals(null, Session::get('baz'));
        $this->assertFalse(Session::has('map.d'));
    }

}