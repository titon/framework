<?hh // strict
namespace Titon\Utility\State;

use Titon\Test\TestCase;

class RequestTest extends TestCase {

    public function testClass(): void {
        $_REQUEST = [
            'foo' => 123,
            'bar' => 'abc',
            'list' => [1, 2, 3],
            'map' => [
                'a' => 'A',
                'b' => 'B',
                'c' => 'C'
            ]
        ];

        Request::initialize($_REQUEST);

        $this->assertEquals(Map {
            'foo' => 123,
            'bar' => 'abc',
            'list' => Vector {1, 2, 3},
            'map' => Map {
                'a' => 'A',
                'b' => 'B',
                'c' => 'C'
            }
        }, Request::all());

        $this->assertEquals(123, Request::get('foo'));
        $this->assertEquals('A', Request::get('map.a'));
        $this->assertEquals(Vector {1, 2, 3}, Request::get('list'));

        $this->assertTrue(Request::has('bar'));
        $this->assertTrue(Request::has('map.b'));

        $this->assertEquals(null, Request::get('baz'));
        $this->assertFalse(Request::has('map.d'));
    }

    public function testStatesDontCollide(): void {
        Get::initialize(['foo' => 1]);
        Post::initialize(['bar' => 2]);
        Server::initialize(['baz' => 3]);

        $this->assertEquals(Map {'foo' => 1}, Get::all());
        $this->assertEquals(Map {'bar' => 2}, Post::all());
        $this->assertEquals(Map {'baz' => 3}, Server::all());
    }

}
