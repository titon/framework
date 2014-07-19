<?hh
namespace Titon\View\View;

use Titon\Test\TestCase;
use Titon\View\View\Engine\AbstractEngine;

/**
 * @property \Titon\View\View\Engine $object
 */
class EngineTest extends TestCase {

    protected function setUp() {
        parent::setUp();

        $this->object = new EngineStub();
    }

    public function testLayout() {
        $this->assertEquals('default', $this->object->getLayout());

        $this->object->useLayout('alternate');
        $this->assertEquals('alternate', $this->object->getLayout());
    }

    public function testWrapper() {
        $this->assertEquals(Vector {}, $this->object->getWrapper());

        $this->object->wrapWith('alternate');
        $this->assertEquals(Vector {'alternate'}, $this->object->getWrapper());

        $this->object->wrapWith('alternate', 'double');
        $this->assertEquals(Vector {'alternate', 'double'}, $this->object->getWrapper());
    }

    public function testContent() {
        $this->assertEquals('', $this->object->getContent());

        $this->object->setContent('content');
        $this->assertEquals('content', $this->object->getContent());
    }

}

class EngineStub extends AbstractEngine {

    public function open(string $partial, Map<string, mixed> $variables = Map {}): string {}
    public function render(string $path, Map<string, mixed> $variables = Map {}): string {}
    public function getExtension(): string {}

}