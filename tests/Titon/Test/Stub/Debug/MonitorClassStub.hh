<?hh
namespace Titon\Test\Stub\Debug;

use Titon\Annotation\WiresAnnotations;

<<Monitor('Titon\Test\Stub\Debug\MonitorClassStub::testCallback')>>
class MonitorClassStub {
    use WiresAnnotations;

    public static string $triggered = '';

    public function __construct() {
        $this->wireClassAnnotation('Monitor');
    }

    public static function testCallback<T>(T $class, string $method = ''): void {
        static::$triggered = get_class($class);
    }
}
