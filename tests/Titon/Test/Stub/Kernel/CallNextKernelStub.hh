<?hh // strict
namespace Titon\Test\Stub\Kernel;

use Titon\Kernel\Input;
use Titon\Kernel\Middleware\Next;
use Titon\Kernel\Output;

class CallNextKernelStub extends KernelStub {
    public function handle(Input $input, Output $output, Next $next): Output {
        $next->handle($input, $output);
        $output->ran = true;

        return $output;
    }
}
