<?hh
namespace Titon\Annotation;

use Titon\Test\TestCase;

class AnnotationTest extends TestCase {

    protected function setUp(): void {
        parent::setUp();

        Registry::map('Foo', 'Titon\Test\Stub\Annotation\FooAnnotationStub');
        Registry::map('Bar', 'Titon\Test\Stub\Annotation\BarAnnotationStub');
        Registry::map('Baz', 'Titon\Test\Stub\Annotation\BazAnnotationStub');
    }

    public function testGetSetName(): void {
        $anno = new Annotation();

        $this->assertEquals('', $anno->getName());

        $anno->setName('Foo');

        $this->assertEquals('Foo', $anno->getName());
    }

}
