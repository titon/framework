<?hh // strict
namespace Titon\Test\Stub\Kernel;

use Titon\Kernel\Middleware\Next;

class InterruptMiddlewareStub extends MiddlewareStub {
    public function handle<Ti, To>(Ti $input, To $output, Next $next): To {
        if ($input instanceof InputStub) {
            $input->stack[] = $this->key;
        }

        return $output;
    }
}
