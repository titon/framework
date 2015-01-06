<?hh
namespace Titon\Common;

use Titon\Test\TestCase;

/**
 * @property \Titon\Common\AnnotateableStub $object
 */
class AnnotateableTest extends TestCase {

    protected function setUp() {
        parent::setUp();

        $this->object = new AnnotateableStub();
    }

    public function testGetAnnotatedMethods() {
        $this->assertEquals(Map {
            'method' => Map {
                'Baz' => Vector{
                    Map {'a' => 1}
                }
            }
        }, $this->object->getAnnotatedMethods());
    }

    public function testGetClassAnnotation() {
        $this->assertEquals(Vector {}, $this->object->getClassAnnotation('Foo'));
        $this->assertEquals(Vector {'abc', 123}, $this->object->getClassAnnotation('Bar'));
    }

    /**
     * @expectedException \Titon\Common\Exception\MissingAnnotationException
     */
    public function testGetClassAnnotationThrowsMissingError() {
        $this->object->getClassAnnotation('Baz');
    }

    public function testGetClassAnnotations() {
        $this->assertEquals(Map {
            'Foo' => Vector {},
            'Bar' => Vector {'abc', 123}
        }, $this->object->getClassAnnotations());
    }

    public function testGetMethodAnnotation() {
        $this->assertEquals(Vector {Map {'a' => 1}}, $this->object->getMethodAnnotation('method', 'Baz'));
    }

    /**
     * @expectedException \Titon\Common\Exception\MissingAnnotationException
     */
    public function testGetMethodAnnotationThrowsMissingError() {
        $this->object->getMethodAnnotation('method', 'Foo');
    }

    public function testGetMethodAnnotations() {
        $this->assertEquals(Map {
            'Baz' => Vector{
                Map {'a' => 1}
            }
        }, $this->object->getMethodAnnotations('method'));
    }

    /**
     * @expectedException \ReflectionException
     */
    public function testGetMethodAnnotationsInvalidMethod() {
        $this->object->getMethodAnnotations('invalid');
    }

}

<<
Foo,
Bar('abc', 123)
>>
class AnnotateableStub implements Annotator {
    use Annotateable;

    <<Baz(['a' => 1])>>
    public function method(): void {}

    public function noAnnoMethod(): void {}
}