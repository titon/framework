<?hh
namespace Titon\Common\Bag;

use Titon\Common\Cacheable;
use Titon\Test\TestCase;

/**
 * @property \Titon\Common\Bag\ReflectionBag $object
 */
class ReflectionBagTest extends TestCase {

    protected function setUp() {
        parent::setUp();

        $this->object = new ReflectionBag(new ReflectionStub());
    }

    public function testGet() {
        $this->assertFalse($this->object->has('className'));
        $this->assertEquals('Titon\Common\Bag\ReflectionStub', $this->object->get('className'));

        $this->assertTrue($this->object->has('className'));
        $this->assertEquals('Titon\Common\Bag\ReflectionStub', $this->object->get('className'));
    }

    public function testReflection() {
        $this->assertInstanceOf('ReflectionClass', $this->object->reflection());
    }

    public function testClassName() {
        $this->assertEquals('Titon\Common\Bag\ReflectionStub', $this->object->className);
        $this->assertEquals('Titon\Common\Bag\ReflectionStub', $this->object->className());
    }

    public function testShortClassName() {
        $this->assertEquals('ReflectionStub', $this->object->shortClassName);
        $this->assertEquals('ReflectionStub', $this->object->shortClassName());
    }

    public function testNamespaceName() {
        $this->assertEquals('Titon\Common\Bag', $this->object->namespaceName);
        $this->assertEquals('Titon\Common\Bag', $this->object->namespaceName());
    }

    public function testFilePath() {
        $this->assertEquals('Titon/Common/Bag/ReflectionStub.php', $this->object->filePath);
        $this->assertEquals('Titon/Common/Bag/ReflectionStub.php', $this->object->filePath());
    }

    public function testMethods() {
        $methods = Vector {
            'publicMethod', 'protectedMethod', 'privateMethod', 'staticPublicMethod', 'staticProtectedMethod', 'staticPrivateMethod',
            'allCache', 'getCache', 'setCache', 'toggleCache', 'removeCache', 'hasCache', 'createCacheKey', 'flushCache', 'cache',
            'serialize', 'unserialize', 'isCacheEnabled'
        };

        $this->assertVectorsEqual($methods, $this->object->methods);
        $this->assertVectorsEqual($methods, $this->object->methods());
    }

    public function testPublicMethods() {
        $methods = Vector {
            'publicMethod', 'staticPublicMethod', 'allCache', 'getCache', 'setCache', 'toggleCache', 'removeCache', 'hasCache', 'createCacheKey', 'flushCache', 'cache',
            'serialize', 'unserialize', 'isCacheEnabled'
        };

        $this->assertVectorsEqual($methods, $this->object->publicMethods);
        $this->assertVectorsEqual($methods, $this->object->publicMethods());
    }

    public function testProtectedMethods() {
        $methods = Vector {'protectedMethod', 'staticProtectedMethod'};

        $this->assertVectorsEqual($methods, $this->object->protectedMethods);
        $this->assertVectorsEqual($methods, $this->object->protectedMethods());
    }

    public function testPrivateMethods() {
        $methods = Vector {'privateMethod', 'staticPrivateMethod'};

        $this->assertVectorsEqual($methods, $this->object->privateMethods);
        $this->assertVectorsEqual($methods, $this->object->privateMethods());
    }

    public function testStaticMethods() {
        $methods = Vector {'staticPublicMethod', 'staticProtectedMethod', 'staticPrivateMethod'};

        $this->assertVectorsEqual($methods, $this->object->staticMethods);
        $this->assertVectorsEqual($methods, $this->object->staticMethods());
    }

    public function testProperties() {
        $props = Vector {
            'publicProp', 'protectedProp', 'privateProp', 'staticPublicProp', 'staticProtectedProp', 'staticPrivateProp',
            '_cache', '_cacheEnabled'
        };

        $this->assertVectorsEqual($props, $this->object->properties);
        $this->assertVectorsEqual($props, $this->object->properties());
    }

    public function testPublicProperties() {
        $props = Vector {'publicProp', 'staticPublicProp'};

        $this->assertVectorsEqual($props, $this->object->publicProperties);
        $this->assertVectorsEqual($props, $this->object->publicProperties());
    }

    public function testProtectedProperties() {
        $props = Vector {'protectedProp', 'staticProtectedProp', '_cache', '_cacheEnabled'};

        $this->assertVectorsEqual($props, $this->object->protectedProperties);
        $this->assertVectorsEqual($props, $this->object->protectedProperties());
    }

    public function testPrivateProperties() {
        $props = Vector {'privateProp', 'staticPrivateProp'};

        $this->assertVectorsEqual($props, $this->object->privateProperties);
        $this->assertVectorsEqual($props, $this->object->privateProperties());
    }

    public function testStaticProperties() {
        $props = Vector {'staticPublicProp', 'staticProtectedProp', 'staticPrivateProp'};

        $this->assertVectorsEqual($props, $this->object->staticProperties);
        $this->assertVectorsEqual($props, $this->object->staticProperties());
    }

    public function testConstants() {
        $constants = Map {'NO' => false, 'YES' => true};

        $this->assertMapsEqual($constants, $this->object->constants, true);
        $this->assertMapsEqual($constants, $this->object->constants(), true);
    }

    public function testTraits() {
        $traits = Vector {'Titon\Common\Cacheable'};

        $this->assertVectorsEqual($traits, $this->object->traits);
        $this->assertVectorsEqual($traits, $this->object->traits());
    }

    public function testInterfaces() {
        $interfaces = Vector {'Serializable'};

        $this->assertVectorsEqual($interfaces, $this->object->interfaces);
        $this->assertVectorsEqual($interfaces, $this->object->interfaces());
    }

    public function testParent() {
        $object = new ReflectionBag(new ReflectionParentStub());

        $this->assertEquals('Titon\Common\Bag\ReflectionStub', $object->parent);
        $this->assertEquals('Titon\Common\Bag\ReflectionStub', $object->parent());
    }

}

class ReflectionStub implements \Serializable {
    use Cacheable;

    const YES = true;
    const NO = false;

    public $publicProp;
    protected $protectedProp;
    private $privateProp;

    public static $staticPublicProp;
    protected static $staticProtectedProp;
    private static $staticPrivateProp;

    public function publicMethod() { }
    protected function protectedMethod() { }
    private function privateMethod() { }

    public static function staticPublicMethod() { }
    protected static function staticProtectedMethod() { }
    private static function staticPrivateMethod() { }

    public function serialize() {}
    public function unserialize($data) {}
}

class ReflectionParentStub extends ReflectionStub {
}