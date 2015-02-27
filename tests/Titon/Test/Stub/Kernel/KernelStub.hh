<?hh // strict
namespace Titon\Test\Stub\Kernel;

use Titon\Kernel\AbstractKernel;
use Titon\Kernel\Middleware\Next;

class KernelStub extends AbstractKernel<InputStub, OutputStub> {
    public function handle<Ti, To>(Ti $input, To $output, Next $next): To {
        if ($input instanceof InputStub) {
            $input->stack[] = 'kernel';
        }

        if ($output instanceof OutputStub) {
            $output->ran = true;
        }

        // UNSAFE
        return $output;
    }
}
