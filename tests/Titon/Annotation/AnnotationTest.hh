<?hh
namespace Titon\Annotation;

use Titon\Test\TestCase;

class AnnotationTest extends TestCase {

    protected function setUp() {
        parent::setUp();

        Registry::map('Foo', 'Titon\Annotation\FooAnnotation');
        Registry::map('Bar', 'Titon\Annotation\BarAnnotation');
        Registry::map('Baz', 'Titon\Annotation\BazAnnotation');
    }

    public function testGetSetName() {
        $anno = new Annotation();

        $this->assertEquals('', $anno->getName());

        $anno->setName('Foo');

        $this->assertEquals('Foo', $anno->getName());
    }

}

// These should be available in the other tests

class FooAnnotation extends Annotation {}

class BarAnnotation extends Annotation {
    public function __construct(public string $string, public int $int = 0) {}
}

class BazAnnotation extends Annotation {
    public function __construct(public array<mixed, mixed> $array) {}
}
