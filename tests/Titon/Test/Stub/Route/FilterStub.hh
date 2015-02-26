<?hh
namespace Titon\Test\Stub\Route;

use Titon\Route\Filter;
use Titon\Route\Route;
use Titon\Route\Router;

class FilterStub implements Filter {
    public function filter(Router $router, Route $route): void {
        return;
    }
}
