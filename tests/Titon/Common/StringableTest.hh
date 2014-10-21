<?hh
namespace Titon\Common;

use Titon\Test\TestCase;

class StringableTest extends TestCase {

    public function testToString() {
        $object = new StringableStub();

        $this->assertEquals('Titon\Common\StringableStub', $object->toString());
        $this->assertEquals('Titon\Common\StringableStub', (string) $object);
    }

}

class StringableStub {
    use Stringable;
}