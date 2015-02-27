<?hh
namespace Titon\Common;

use Titon\Test\Stub\Common\StringableStub;
use Titon\Test\TestCase;

class StringableTest extends TestCase {

    public function testToString(): void {
        $object = new StringableStub();

        $this->assertEquals('Titon\Test\Stub\Common\StringableStub', $object->toString());
        $this->assertEquals('Titon\Test\Stub\Common\StringableStub', (string) $object);
    }

}
