<?hh // strict
namespace Titon\Test\Stub\Kernel;

use Titon\Kernel\Input;

class InputStub implements Input {
    public array<mixed> $stack = [];
}
