<?hh
namespace Titon\Route;

use Titon\Test\TestCase;

class CallbackRouteTest extends TestCase {

    public function testDispatch() {
        $route = new CallbackRoute('/{a}/{b}', function(string $a, string $b): string {
            return $a . $b;
        });
        $route->isMatch('/foo/bar');

        $this->assertEquals('foobar', $route->dispatch());
    }

    public function testDispatchOptional() {
        $route = new CallbackRoute('/{a}/{b?}', function(string $a, string $b = 'baz'): string {
            return $a . $b;
        });
        $route->isMatch('/foo');

        $this->assertEquals('foobaz', $route->dispatch());
    }

    public function testDispatchTypeHints() {
        $route = new CallbackRoute('/{a}/[b]/(c)', function(string $a, int $b, string $c): string {
            return $a . $b . $c;
        });
        $route->isMatch('/foo/123/bar_456');

        $this->assertEquals('foo123bar_456', $route->dispatch());
    }

    /**
     * @expectedException \Titon\Route\Exception\NoMatchException
     */
    public function testDispatchNoMatch() {
        $route = new CallbackRoute('/', function(): string { return ''; });
        $route->dispatch();
    }

}