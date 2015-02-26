<?hh
namespace Titon\Annotation;

use Titon\Test\TestCase;

class RegistryTest extends TestCase {

    public function testMapAndFactory() {
        $annotation = Registry::factory('Bar', ['This is the message!']);

        $this->assertInstanceOf('Titon\Annotation\Annotation', $annotation);
        $this->assertInstanceOf('Titon\Test\Stub\Annotation\BarAnnotationStub', $annotation);
        $this->assertEquals('Bar', $annotation->getName());
        $this->assertEquals('This is the message!', $annotation->string);
    }

    /**
     * @expectedException \Titon\Annotation\Exception\InvalidClassException
     */
    public function testMapThrowsException() {
        Registry::map('Qux', 'Titon\Annotation\Reader');
    }

    /**
     * @expectedException \Titon\Annotation\Exception\MissingAnnotationException
     */
    public function testFactoryThrowsException() {
        Registry::factory('Qux', []);
    }

    public function testArgsArePassedToAnnotation() {
        $bar = Registry::factory('Bar', ['abc']);

        $this->assertEquals('abc', $bar->string);
        $this->assertEquals(0, $bar->int);

        $bar = Registry::factory('Bar', ['def', 123]);

        $this->assertEquals('def', $bar->string);
        $this->assertEquals(123, $bar->int);

        $baz = Registry::factory('Baz', [[1, 2, 3]]);

        $this->assertEquals([1, 2, 3], $baz->array);
    }

}
