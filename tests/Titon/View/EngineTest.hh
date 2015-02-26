<?hh // strict
namespace Titon\View;

use Titon\Test\Stub\View\EngineStub;
use Titon\Test\TestCase;

/**
 * @property \Titon\View\Engine $object
 */
class EngineTest extends TestCase {

    protected function setUp(): void {
        parent::setUp();

        $this->object = new EngineStub();
    }

    public function testLayout(): void {
        $this->assertEquals('default', $this->object->getLayout());

        $this->object->useLayout('alternate');
        $this->assertEquals('alternate', $this->object->getLayout());
    }

    public function testWrappers(): void {
        $this->assertEquals(Vector {}, $this->object->getWrappers());

        $this->object->wrapWith('alternate');
        $this->assertEquals(Vector {'alternate'}, $this->object->getWrappers());

        $this->object->wrapWith('alternate', 'double');
        $this->assertEquals(Vector {'alternate', 'double'}, $this->object->getWrappers());
    }

    public function testContent(): void {
        $this->assertEquals('', $this->object->getContent());

        $this->object->setContent('content');
        $this->assertEquals('content', $this->object->getContent());
    }

}
