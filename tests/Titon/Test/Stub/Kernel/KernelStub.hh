<?hh // strict
namespace Titon\Test\Stub\Kernel;

use Titon\Kernel\AbstractKernel;
use Titon\Kernel\Middleware\Next;

class KernelStub extends AbstractKernel<ApplicationStub, InputStub, OutputStub> {
    public function handle(InputStub $input, OutputStub $output, Next<InputStub, OutputStub> $next): OutputStub {
        $input->stack[] = 'kernel';
        $output->ran = true;

        return $output;
    }
}
