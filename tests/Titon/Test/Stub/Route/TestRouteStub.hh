<?hh // strict
namespace Titon\Test\Stub\Route;

use Titon\Route\Route;

class TestRouteStub extends Route {
    public function __construct(string $path, string $action): void {
        parent::__construct($path, $action);

        $this->compile();
    }
}
