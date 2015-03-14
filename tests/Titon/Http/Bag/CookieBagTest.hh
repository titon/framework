<?hh
namespace Titon\Http\Bag;

use Titon\Test\TestCase;

class CookieBagTest extends TestCase {

    public function testMapItemsAreConvertedToClasses(): void {
        $bag = new CookieBag(Map {
            'foo' => '123',
            'bar' => '456',
            'baz' => '789'
        });

        foreach ($bag->all() as $cookie) {
            $this->assertInstanceOf('Titon\Http\Cookie', $cookie);
        }
    }

}
