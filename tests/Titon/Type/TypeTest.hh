<?hh
namespace Titon\Type;

use Titon\Test\TestCase;

class TypeTest extends TestCase {

    public function testIs() {
        $this->assertEquals('array', Type::is([]));
        $this->assertEquals('object', Type::is(new \stdClass()));
        $this->assertEquals('map', Type::is(Map {}));
        $this->assertEquals('vector', Type::is(Vector {}));
        $this->assertEquals('set', Type::is(Set {}));
        $this->assertEquals('pair', Type::is(Pair {1, 2}));
        $this->assertEquals('boolean', Type::is(true));
        $this->assertEquals('integer', Type::is(123));
        $this->assertEquals('double', Type::is(12.3));
        $this->assertEquals('string', Type::is('foo'));
    }

    public function testIsResource() {
        $f = fopen('php://input', 'r');

        $this->assertEquals('resource', Type::is($f));

        fclose($f);
    }

}