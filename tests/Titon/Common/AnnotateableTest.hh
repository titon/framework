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
        $this->assertVectorsEqual(Vector {
            'method',
            'getMethodAnnotations',
            'getMethodAnnotations$memoize_impl', // Hack
            'getAnnotatedMethods',
            'getAnnotatedMethods$memoize_impl', // Hack
            'getClassAnnotations',
            'getClassAnnotations$memoize_impl', // Hack
        }, $this->object->getAnnotatedMethods());
    }

    public function testGetClassAnnotation() {
        $this->assertEquals(Vector {}, $this->object->getClassAnnotation('Foo'));
        $this->assertEquals(Vector {'abc', 123}, $this->object->getClassAnnotation('Bar'));
        $this->assertEquals(Vector {}, $this->object->getClassAnnotation('Baz'));
    }

    public function testGetClassAnnotations() {
        $this->assertEquals(Map {
            'Foo' => Vector {},
            'Bar' => Vector {'abc', 123}
        }, $this->object->getClassAnnotations());
    }

    public function testGetMethodAnnotation() {
        $this->assertEquals(Vector {}, $this->object->getMethodAnnotation('method', 'Foo'));
        $this->assertEquals(Vector {}, $this->object->getMethodAnnotation('method', 'Bar'));
        $this->assertEquals(Vector {Map {'a' => 1}}, $this->object->getMethodAnnotation('method', 'Baz'));
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