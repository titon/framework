<?hh // strict
namespace Titon\Test\Stub\Kernel;

use Titon\Kernel\Middleware;
use Titon\Kernel\Middleware\Next;

class MiddlewareStub implements Middleware<InputStub, OutputStub> {
    public function __construct(protected string $key): void {}

    public function handle(InputStub $input, OutputStub $output, Next<InputStub, OutputStub> $next): OutputStub {
        $input->stack[] = $this->key;

        $output = $next->handle($input, $output);

        $input->stack[] = $this->key;

        return $output;
    }
}
