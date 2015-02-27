<?hh // strict
namespace Titon\Test\Stub\Route;

<<Route('parent', '/controller')>>
class RouteAnnotatedStub {

    <<Route('foo', '/foo')>>
    public function foo(): void {}

    <<Route('bar', '/bar', 'POST')>>
    public function bar(): void {}

    <<Route('baz', '/baz', ['get'], ['auth', 'guest'])>>
    public function baz(): void {}

    <<Route('qux', '/qux', ['PUT', 'POST'], [], ['id' => '[1-8]+'])>>
    public function qux(): void {}

}
