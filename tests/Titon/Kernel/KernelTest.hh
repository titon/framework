<?hh
namespace Titon\Kernel;

use Titon\Kernel\Middleware\Next;
use Titon\Kernel\Middleware\Pipeline;
use Titon\Test\TestCase;

/**
 * @property \Titon\Kernel\KernelStub $object
 * @property \Titon\Kernel\InputStub $input
 * @property \Titon\Kernel\OutputStub $output
 */
class KernelTest extends TestCase {

    protected function setUp() {
        parent::setUp();

        $this->object = new KernelStub(new Pipeline());
        $this->input = new InputStub();
        $this->output = new OutputStub();
    }

    public function testKernelModifiesOutputInHandle() {
        $this->assertFalse($this->output->ran);

        $this->object->run($this->input, $this->output);

        $this->assertTrue($this->output->ran);
    }

    public function testKernelRunsWithoutMiddleware() {
        $this->object = new KernelStub(new Pipeline());

        $this->assertFalse($this->output->ran);

        $this->object->run($this->input, $this->output);

        $this->assertTrue($this->output->ran);
    }

    public function testKernelCallNextHandleDoesNothing() {
        $this->object = new CallNextKernelStub(new Pipeline());
        $this->object->pipe(new MiddlewareStub('foo'));
        $this->object->pipe(new MiddlewareStub('bar'));
        $this->object->pipe(new MiddlewareStub('baz'));

        $this->assertFalse($this->output->ran);

        $this->object->run($this->input, $this->output);

        $this->assertTrue($this->output->ran);
    }

    public function testMiddlewareAreExecutedInANestedFormat() {
        $this->object->pipe(new MiddlewareStub('foo'));
        $this->object->pipe(new MiddlewareStub('bar'));
        $this->object->pipe(new MiddlewareStub('baz'));

        $this->assertEquals([], $this->input->stack);

        $this->object->run($this->input, $this->output);

        $this->assertEquals(['foo', 'bar', 'baz', 'kernel', 'baz', 'bar', 'foo'], $this->input->stack);
    }

    public function testMiddlewareCanInterruptCycle() {
        $this->object->pipe(new MiddlewareStub('foo'));
        $this->object->pipe(new InterruptMiddlewareStub('bar'));
        $this->object->pipe(new MiddlewareStub('baz'));

        $this->assertEquals([], $this->input->stack);

        $this->object->run($this->input, $this->output);

        $this->assertEquals(['foo', 'bar', 'foo'], $this->input->stack);
    }

    public function testExecutionTimeIsLogged() {
        $this->object->run($this->input, $this->output);

        $this->assertLessThan($this->object->getStartTime(), $this->object->getExecutionTime());
    }

}

class KernelStub extends AbstractKernel<InputStub, OutputStub> {
    public function handle(Input $input, Output $output, Next $next): Output {
        $input->stack[] = 'kernel';
        $output->ran = true;

        return $output;
    }
}

class CallNextKernelStub extends KernelStub {
    public function handle(Input $input, Output $output, Next $next): Output {
        $next->handle($input, $output);
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

    public function handle<Ti, To>(Ti $input, To $output, Next $next): To {
        $input->stack[] = $this->key;

        $output = $next->handle($input, $output);

        $input->stack[] = $this->key;

        return $output;
    }
}

class InterruptMiddlewareStub extends MiddlewareStub {
    public function handle<Ti, To>(Ti $input, To $output, Next $next): To {
        $input->stack[] = $this->key;

        return $output;
    }
}
