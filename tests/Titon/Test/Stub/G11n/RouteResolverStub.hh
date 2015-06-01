<?hh // strict
namespace Titon\Test\Stub\G11n;

use Titon\G11n\RouteResolver;

class RouteResolverStub extends RouteResolver {

    public function redirect(string $redirectTo): void {
        echo $redirectTo;
    }

}
