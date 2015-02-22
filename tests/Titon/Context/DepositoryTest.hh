<?hh
/**
 * @author Alex Phillips <exonintrendo@gmail.com>
 * Date: 2/5/15
 * Time: 6:05 PM
 */

namespace Titon\Context;

use Titon\Context\ServiceProvider\AbstractServiceProvider;
use Titon\Test\TestCase;

/**
 * @property \Titon\Context\Depository $container
 */
class DepositoryTest extends TestCase
{
    protected function setUp()
    {
        parent::setUp();

        $this->container = new Depository();
    }

    public function testRegisterClassName()
    {
        $this->container->register('foo', 'Titon\Context\Foo');

        $this->assertTrue($this->container->isRegistered('foo'));
        $this->assertTrue($this->container->isRegistered('Titon\Context\Foo'));
        $this->assertInstanceOf('Titon\Context\Foo', $this->container->make('foo'));
        $this->assertInstanceOf('Titon\Context\Foo', $this->container->make('Titon\Context\Foo'));
    }

    public function testRegisterInstance()
    {
        $this->container->register('foo', new Foo());
        $this->assertInstanceOf('Titon\Context\Foo', $this->container->make('foo'));
        $this->assertTrue($this->container->isSingleton('foo'));
        $this->assertSame($this->container->make('foo'), $this->container->make('Titon\Context\Foo'));
    }

    public function testRegisterClosure()
    {
        $this->container->register('foo', function(){
            $foo = new Foo('Foo');

            return $foo;
        });

        $this->assertInstanceOf('Titon\Context\Foo', $this->container->make('foo'));
        $this->assertEquals('Foo', $this->container->make('foo')->getName());
    }

    public function testRegisterCallable()
    {
        $this->container->register('foo', ['Titon\Context\Foo', 'factory']);
        $this->assertInstanceOf('Titon\Context\Foo', $this->container->make('foo'));

        $this->container->register('Foo', 'Titon\Context\Foo::factory');
        $this->assertInstanceOf('Titon\Context\Foo', $this->container->make('Foo'));
    }

    public function testAutoDependencyResolution()
    {
        $test = $this->container->make('Titon\Context\Bar');
        $this->assertInstanceOf('Titon\Context\Bar', $test);
        $this->assertInstanceOf('Titon\Context\Foo', $test->getFoo());
    }

    public function testSingletonNestedResolution()
    {
        $this->container->singleton('foo', 'Titon\Context\Foo');
        $foo = $this->container->make('foo');
        $bar = $this->container->make('Titon\Context\Bar');
        $this->assertSame($foo, $bar->getFoo());
        $this->assertSame($foo, $this->container->make('Titon\Context\Foo'));
    }

    public function testMethodInjection()
    {
        $this->container->register('foo', 'Titon\Context\Foo')->call('setName', 'Foo Bar');
        $this->assertInstanceOf('Titon\Context\Foo', $this->container->make('foo'));
        $this->assertEquals('Foo Bar', $this->container->make('foo')->getName());

        $this->container->remove('foo');

        $this->container->register('foo', 'Titon\Context\Foo')->call('setBar', new Bar(new Foo('Alex Phillips')));
        $this->assertInstanceOf('Titon\Context\Foo', $this->container->make('foo'));
        $this->assertEquals('Alex Phillips', $this->container->make('foo')->getName());
    }

    public function testMethodChaining()
    {
        $test = $this->container->make('Titon\Context\Bar')->getFoo();
        $this->assertInstanceOf('Titon\Context\Foo', $test);
    }

    public function testConstructorInjection()
    {
        $this->container->register('bar', 'Titon\Context\Bar')->with('Titon\Context\Foo');
        $this->assertInstanceOf('Titon\Context\Bar', $this->container->make('bar'));

        $this->container->remove('bar');

        $this->container->register('bar', 'Titon\Context\Bar')->with((new Foo())->setName('Foo Bar'));
        $this->assertInstanceOf('Titon\Context\Bar', $this->container->make('bar'));
        $this->assertEquals($this->container->make('bar')->getFoo()->getName(), 'Foo Bar');
    }

    public function testPassArgumentsAtMake()
    {
        $this->container->register('bar', 'Titon\Context\Bar');
        $test = $this->container->make('bar', new Foo('Foo Bar'));
        $this->assertInstanceOf('Titon\Context\Bar', $test);
        $this->assertEquals('Foo Bar', $test->getFoo()->getName());
    }

    public function testAliasing()
    {
        $this->container->register('foo', 'Titon\Context\Foo', true);
        $this->container->alias('foobar', 'Titon\Context\Foo');

        $this->assertSame($this->container->make('foo'), $this->container->make('foobar'));
        $this->assertSame($this->container->make('foobar'), $this->container->make('Titon\Context\Foo'));
    }

    public function testAliasChaining()
    {
        $this->container->register('Titon\Context\Foo')->alias('foo');
        $this->assertInstanceOf('Titon\Context\Foo', $this->container->make('Titon\Context\Foo'));
        $this->assertInstanceOf('Titon\Context\Foo', $this->container->make('foo'));
    }

    public function testNestedDependencyResolution()
    {
        $test = $this->container->make('Titon\Context\Bar');
        $this->assertEquals('Alex Phillips', $test->getFoo()->getName());
    }

    public function testCallableResolution()
    {
        $this->assertEquals('Alex Phillips', $this->container->make('Titon\Context\FooBar'));
    }

    public function testClosureDependencyResolution()
    {
        $bar = $this->container->make(function (Foo $foo) {
            return new Bar($foo);
        });

        $this->assertEquals('Alex Phillips', $bar->getFoo()->getName());
    }

    public function testMakeSingleton()
    {
        $this->container->register('Titon\Context\Foo');
        $this->assertNotSame($this->container->make('Titon\Context\Foo'), $this->container->make('Titon\Context\Foo'));

        $this->container->makeSingleton('Titon\Context\Foo');
        $this->assertSame($this->container->make('Titon\Context\Foo'), $this->container->make('Titon\Context\Foo'));
    }

    public function testServiceProviders()
    {
        $this->container->addServiceProvider('Titon\Context\SomeServiceProvider');
        $this->assertEquals(new Foo(), $this->container->make('Titon\Context\Foo'));
        $this->assertEquals(new Foo(), $this->container->make('foo'));

        $this->container->addServiceProvider('Titon\Context\AnotherServiceProvider');
        $this->assertSame($this->container->make('bar'), $this->container->make('Titon\Context\Bar'));
    }
}

class Foo
{
    public $name;

    private $bar;

    public function __construct($name = 'Alex Phillips')
    {
        $this->name = $name;
    }

    public static function factory($name = 'Alex Phillips')
    {
        return new Foo($name);
    }

    public function getName()
    {
        return $this->name;
    }

    public function setName($name)
    {
        $this->name = $name;

        return $this;
    }

    public function setBar(Bar $bar)
    {
        $this->bar = $bar;
    }
}

class Bar
{
    private $foo;

    public function __construct(Foo $foo)
    {
        $this->foo = $foo;
    }

    public function getFoo()
    {
        return $this->foo;
    }

    public function setFoo($foo)
    {
        $this->foo = $foo;
    }
}

function FooBar(Foo $foo) {
    return $foo->getName();
}

class SomeServiceProvider extends AbstractServiceProvider {

    protected ClassList $provides = Vector {
        'Titon\Context\Foo'
    };

    public function register() {
        $this->depository->register('foo', 'Titon\Context\Foo');
    }
}

class AnotherServiceProvider extends AbstractServiceProvider {

    public function register() {
        $this->depository->singleton('bar', 'Titon\Context\Bar')->with('Titon\Context\Foo');
    }
}