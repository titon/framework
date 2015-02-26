<?hh
namespace Titon\Test\Stub\Kernel;

use Titon\Kernel\AbstractKernel;
use Titon\Kernel\Input;
use Titon\Kernel\Middleware\Next;
use Titon\Kernel\Output;

class KernelStub extends AbstractKernel<InputStub, OutputStub> {
    public function handle(Input $input, Output $output, Next $next): Output {
        $input->stack[] = 'kernel';
        $output->ran = true;

        return $output;
    }
}
