<?hh
namespace Titon\Type;

use Titon\Test\TestCase;

/**
 * @property \Titon\Type\ArrayList $object
 */
class ArrayListTest extends TestCase {

    protected function setUp() {
        parent::setUp();

        $this->object = new ArrayList(Vector {'foo'});
    }

    public function testValue() {
        $this->assertEquals(Vector {'foo'}, $this->object->value());
    }

}