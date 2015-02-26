<?hh // strict
namespace Titon\Http\Bag;

use Titon\Http\Cookie;
use Titon\Test\TestCase;

class CookieBagTest extends TestCase {

    public function testMapItemsAreConvertedToClasses(): void {
        $bag = new CookieBag(Map {
            'foo' => '123',
            'bar' => '456',
            'baz' => '789'
        });

        $this->assertEquals(Map {
            'foo' => new Cookie('foo', '123'),
            'bar' => new Cookie('bar', '456'),
            'baz' => new Cookie('baz', '789'),
        }, $bag->all());
    }

}
