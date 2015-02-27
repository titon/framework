<?hh
namespace Titon\Kernel;

use Titon\Kernel\Middleware\Pipeline;
use Titon\Test\Stub\Kernel\ApplicationStub;
use Titon\Test\Stub\Kernel\CallNextKernelStub;
use Titon\Test\Stub\Kernel\InputStub;
use Titon\Test\Stub\Kernel\InterruptMiddlewareStub;
use Titon\Test\Stub\Kernel\KernelStub;
use Titon\Test\Stub\Kernel\MiddlewareStub;
use Titon\Test\Stub\Kernel\OutputStub;
use Titon\Test\TestCase;

/**
 * @property \Titon\Test\Stub\Kernel\KernelStub $object
 * @property \Titon\Test\Stub\Kernel\InputStub $input
 * @property \Titon\Test\Stub\Kernel\OutputStub $output
 */
class KernelTest extends TestCase {

    protected function setUp(): void {
        parent::setUp();

        $this->object = new KernelStub(new ApplicationStub(), new Pipeline());
        $this->input = new InputStub();
        $this->output = new OutputStub();
    }

    public function testKernelModifiesOutputInHandle(): void {
        $this->assertFalse($this->output->ran);

        $this->object->run($this->input, $this->output);

        $this->assertTrue($this->output->ran);
    }

    public function testKernelRunsWithoutMiddleware(): void {
        $this->object = new KernelStub(new ApplicationStub(), new Pipeline());

        $this->assertFalse($this->output->ran);

        $this->object->run($this->input, $this->output);

        $this->assertTrue($this->output->ran);
    }

    public function testKernelCallNextHandleDoesNothing(): void {
        $this->object = new CallNextKernelStub(new ApplicationStub(), new Pipeline());
        $this->object->pipe(new MiddlewareStub('foo'));
        $this->object->pipe(new MiddlewareStub('bar'));
        $this->object->pipe(new MiddlewareStub('baz'));

        $this->assertFalse($this->output->ran);

        $this->object->run($this->input, $this->output);

        $this->assertTrue($this->output->ran);
    }

    public function testMiddlewareAreExecutedInANestedFormat(): void {
        $this->object->pipe(new MiddlewareStub('foo'));
        $this->object->pipe(new MiddlewareStub('bar'));
        $this->object->pipe(new MiddlewareStub('baz'));

        $this->assertEquals([], $this->input->stack);

        $this->object->run($this->input, $this->output);

        $this->assertEquals(['foo', 'bar', 'baz', 'kernel', 'baz', 'bar', 'foo'], $this->input->stack);
    }

    public function testMiddlewareCanInterruptCycle(): void {
        $this->object->pipe(new MiddlewareStub('foo'));
        $this->object->pipe(new InterruptMiddlewareStub('bar'));
        $this->object->pipe(new MiddlewareStub('baz'));

        $this->assertEquals([], $this->input->stack);

        $this->object->run($this->input, $this->output);

        $this->assertEquals(['foo', 'bar', 'foo'], $this->input->stack);
    }

    public function testExecutionTimeIsLogged(): void {
        $this->object->run($this->input, $this->output);

        $this->assertLessThan($this->object->getStartTime(), $this->object->getExecutionTime());
    }

}
