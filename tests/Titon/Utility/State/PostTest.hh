<?hh
namespace Titon\Utility\State;

use Titon\Test\TestCase;

class PostTest extends TestCase {

    public function testClass() {
        $_POST = [
            'foo' => 123,
            'bar' => 'abc',
            'list' => [1, 2, 3],
            'map' => [
                'a' => 'A',
                'b' => 'B',
                'c' => 'C'
            ]
        ];

        Post::initialize($_POST);

        $this->assertEquals(Map {
            'foo' => 123,
            'bar' => 'abc',
            'list' => Vector {1, 2, 3},
            'map' => Map {
                'a' => 'A',
                'b' => 'B',
                'c' => 'C'
            }
        }, Post::all());

        $this->assertEquals(123, Post::get('foo'));
        $this->assertEquals('A', Post::get('map.a'));
        $this->assertEquals(Vector {1, 2, 3}, Post::get('list'));

        $this->assertTrue(Post::has('bar'));
        $this->assertTrue(Post::has('map.b'));

        $this->assertEquals(null, Post::get('baz'));
        $this->assertFalse(Post::has('map.d'));
    }

}