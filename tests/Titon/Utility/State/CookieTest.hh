<?hh
namespace Titon\Utility\State;

use Titon\Test\TestCase;

class CookieTest extends TestCase {

    public function testClass(): void {
        $_COOKIE = [
            'foo' => '940a8df7d359963b805f92e125dabecf',
            'bar' => '756457dc85f13450b3dfba2cbc1465e5'
        ];

        Cookie::initialize($_COOKIE);

        $this->assertEquals(Map {
            'foo' => '940a8df7d359963b805f92e125dabecf',
            'bar' => '756457dc85f13450b3dfba2cbc1465e5'
        }, Cookie::all());

        $this->assertEquals('940a8df7d359963b805f92e125dabecf', Cookie::get('foo'));
        $this->assertTrue(Cookie::has('bar'));

        $this->assertEquals(null, Cookie::get('baz'));
        $this->assertFalse(Cookie::has('baz'));
    }

}
