<?hh
namespace Titon\Annotation;

use Titon\Test\Stub\Annotation\BarAnnotationStub;
use Titon\Test\Stub\Annotation\BazAnnotationStub;
use Titon\Test\Stub\Annotation\FooAnnotationStub;
use Titon\Test\Stub\Annotation\ReaderStub;
use Titon\Test\TestCase;

/**
 * @property \Titon\Annotation\Reader $object
 */
class ReaderTest extends TestCase {

    protected function setUp() {
        parent::setUp();

        $this->object = new Reader(new ReaderStub());
    }

    public function testGetAnnotatedMethods() {
        $this->assertEquals(Map {
            'hasAnno' => Map {
                'Baz' => (new BazAnnotationStub(['a' => 1]))->setName('Baz')
            }
        }, $this->object->getAnnotatedMethods());
    }

    public function testGetClassAnnotation() {
        $this->assertEquals((new FooAnnotationStub())->setName('Foo'), $this->object->getClassAnnotation('Foo'));
        $this->assertEquals((new BarAnnotationStub('abc', 123))->setName('Bar'), $this->object->getClassAnnotation('Bar'));
    }

    /**
     * @expectedException \Titon\Annotation\Exception\MissingAnnotationException
     */
    public function testGetClassAnnotationThrowsMissingError() {
        $this->object->getClassAnnotation('Baz');
    }

    public function testGetClassAnnotations() {
        $this->assertEquals(Map {
            'Foo' => (new FooAnnotationStub())->setName('Foo'),
            'Bar' => (new BarAnnotationStub('abc', 123))->setName('Bar')
        }, $this->object->getClassAnnotations());
    }

    public function testGetMethodAnnotation() {
        $this->assertEquals((new BazAnnotationStub(['a' => 1]))->setName('Baz'), $this->object->getMethodAnnotation('hasAnno', 'Baz'));
    }

    /**
     * @expectedException \Titon\Annotation\Exception\MissingAnnotationException
     */
    public function testGetMethodAnnotationThrowsMissingError() {
        $this->object->getMethodAnnotation('hasAnno', 'Foo');
    }

    public function testGetMethodAnnotations() {
        $this->assertEquals(Map {
            'Baz' => (new BazAnnotationStub(['a' => 1]))->setName('Baz')
        }, $this->object->getMethodAnnotations('hasAnno'));
    }

    /**
     * @expectedException \ReflectionException
     */
    public function testGetMethodAnnotationsInvalidMethod() {
        $this->object->getMethodAnnotations('invalid');
    }

}
