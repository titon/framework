<?hh
namespace Titon\Test\Stub\Debug;

use Titon\Annotation\WiresAnnotations;

<<Deprecated('This is the error message.')>>
class DeprecatedClassStub {
    use WiresAnnotations;

    public function __construct() {
        $this->wireClassAnnotation('Deprecated');
    }
}
