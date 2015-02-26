<?hh
namespace Titon\Route;

use Titon\Test\TestCase;

/**
 * @property \Titon\Route\Group $object
 */
class GroupTest extends TestCase {

    protected function setUp(): void {
        parent::setUp();

        $this->object = new Group();
    }

    public function testPrefixing(): void {
        $this->assertEquals('', $this->object->getPrefix());

        $this->object->setPrefix('foo');

        $this->assertEquals('foo', $this->object->getPrefix());
    }

    public function testSuffixing(): void {
        $this->assertEquals('', $this->object->getSuffix());

        $this->object->setSuffix('bar');

        $this->assertEquals('bar', $this->object->getSuffix());
    }

}
