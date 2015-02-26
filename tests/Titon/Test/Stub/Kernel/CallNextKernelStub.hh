<?hh // strict
namespace Titon\Test\Stub\Kernel;

use Titon\Kernel\Middleware\Next;

class CallNextKernelStub extends KernelStub {
    public function handle<Ti, To>(Ti $input, To $output, Next $next): To {
        $next->handle($input, $output);

        if ($output instanceof OutputStub) {
            $output->ran = true;
        }

        // UNSAFE
        return $output;
    }
}
