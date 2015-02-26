<?hh
namespace Titon\Test\Stub\Route;

class DispatchRouteStub {
    public function noOptional(string $a, string $b): string {
        return $a . $b;
    }

    public function withOptional(string $a, string $b = 'baz'): string {
        return $a . $b;
    }

    public function typeHints(string $a, int $b, string $c): string {
        return $a . $b . $c;
    }
}
