<?hh // strict
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

    protected function setUp(): void {
        parent::setUp();

        $this->object = new Reader(new ReaderStub());
    }

    public function testGetAnnotatedMethods(): void {
        $this->assertEquals(Map {
            'hasAnno' => Map {
                'Baz' => (new BazAnnotationStub(['a' => 1]))->setName('Baz')
            }
        }, $this->object->getAnnotatedMethods());
    }

    public function testGetClassAnnotation(): void {
        $this->assertEquals((new FooAnnotationStub())->setName('Foo'), $this->object->getClassAnnotation('Foo'));
        $this->assertEquals((new BarAnnotationStub('abc', 123))->setName('Bar'), $this->object->getClassAnnotation('Bar'));
    }

    /**
     * @expectedException \Titon\Annotation\Exception\MissingAnnotationException
     */
    public function testGetClassAnnotationThrowsMissingError(): void {
        $this->object->getClassAnnotation('Baz');
    }

    public function testGetClassAnnotations(): void {
        $this->assertEquals(Map {
            'Foo' => (new FooAnnotationStub())->setName('Foo'),
            'Bar' => (new BarAnnotationStub('abc', 123))->setName('Bar')
        }, $this->object->getClassAnnotations());
    }

    public function testGetMethodAnnotation(): void {
        $this->assertEquals((new BazAnnotationStub(['a' => 1]))->setName('Baz'), $this->object->getMethodAnnotation('hasAnno', 'Baz'));
    }

    /**
     * @expectedException \Titon\Annotation\Exception\MissingAnnotationException
     */
    public function testGetMethodAnnotationThrowsMissingError(): void {
        $this->object->getMethodAnnotation('hasAnno', 'Foo');
    }

    public function testGetMethodAnnotations(): void {
        $this->assertEquals(Map {
            'Baz' => (new BazAnnotationStub(['a' => 1]))->setName('Baz')
        }, $this->object->getMethodAnnotations('hasAnno'));
    }

    /**
     * @expectedException \ReflectionException
     */
    public function testGetMethodAnnotationsInvalidMethod(): void {
        $this->object->getMethodAnnotations('invalid');
    }

}
