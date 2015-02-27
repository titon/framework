<?hh // strict
namespace Titon\Test\Stub\Kernel;

use Titon\Kernel\Output;

class OutputStub implements Output {
    public bool $ran = false;

    public function send(): void {}
}
