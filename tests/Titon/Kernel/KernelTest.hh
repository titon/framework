<?hh
namespace Titon\Kernel;

use Titon\Kernel\Middleware\Next;
use Titon\Test\TestCase;

/**
 * @property \Titon\Kernel\Kernel $object
 */
class KernelTest extends TestCase {

    protected function setUp() {
        parent::setUp();

        $this->object = new KernelStub();
        $this->object->pipe(new MiddlewareStub('foo'));
        $this->object->pipe(new MiddlewareStub('bar'));
        $this->object->pipe(new MiddlewareStub('baz'));
    }

    public function testKernelModifiesOutputInHandle() {
        $output = new OutputStub();

        $this->assertFalse($output->ran);

        $this->object->run(new InputStub(), $output);

        $this->assertTrue($output->ran);
    }

    public function testMiddlewareAreExecutedInANestedFormat() {
        $input = new InputStub();

        $this->assertEquals([], $input->stack);

        $this->object->run($input, new OutputStub());

        $this->assertEquals(['foo', 'bar', 'baz', 'kernel', 'baz', 'bar', 'foo'], $input->stack);
    }

}

class KernelStub extends AbstractKernel<InputStub, OutputStub> {
    public function handle(Input $input, Output $output, Next $next): Output {
        $input->stack[] = 'kernel';
        $output->ran = true;

        return $output;
    }
}

class InputStub implements Input {
    public array<mixed> $stack = [];
}

class OutputStub implements Output {
    public bool $ran = false;

    public function send(): void {}
}

class MiddlewareStub implements Middleware {
    public function __construct(protected string $key) {}

    public function handle(Input $input, Output $output, Next $next): Output {
        $input->stack[] = $this->key;

        $output = $next->handle($input, $output);

        $input->stack[] = $this->key;

        return $output;
    }
}
