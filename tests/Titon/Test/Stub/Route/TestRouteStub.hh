<?hh
namespace Titon\Test\Stub\Route;

use Titon\Route\Route;

class TestRouteStub extends Route {
    public function __construct(string $path, string $action) {
        parent::__construct($path, $action);

        $this->compile();
    }
}
