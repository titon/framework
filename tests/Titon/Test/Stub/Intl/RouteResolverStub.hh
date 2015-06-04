<?hh // strict
namespace Titon\Test\Stub\Intl;

use Titon\Intl\RouteResolver;

class RouteResolverStub extends RouteResolver {

    public function redirect(string $redirectTo): void {
        echo $redirectTo;
    }

}
