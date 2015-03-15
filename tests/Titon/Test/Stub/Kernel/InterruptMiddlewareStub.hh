<?hh // strict
namespace Titon\Test\Stub\Kernel;

use Titon\Kernel\Middleware\Next;

class InterruptMiddlewareStub extends MiddlewareStub {
    public function handle(InputStub $input, OutputStub $output, Next<InputStub, OutputStub> $next): OutputStub {
        $input->stack[] = $this->key;

        return $output;
    }
}
