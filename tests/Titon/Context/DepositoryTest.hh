<?hh
namespace Titon\Context;

use Titon\Test\Stub\Context\BarStub;
use Titon\Test\Stub\Context\FooStub;
use Titon\Test\TestCase;

/**
 * @property \Titon\Context\Depository $container
 */
class DepositoryTest extends TestCase
{
    protected function setUp(): void {
        parent::setUp();

        $this->container = new Depository();
    }

    public function testRegisterClassName(): void {
        $this->container->register('foo', 'Titon\Test\Stub\Context\FooStub');

        $this->assertTrue($this->container->isRegistered('foo'));
        $this->assertTrue($this->container->isRegistered('Titon\Test\Stub\Context\FooStub'));
        $this->assertInstanceOf('Titon\Test\Stub\Context\FooStub', $this->container->make('foo'));
        $this->assertInstanceOf('Titon\Test\Stub\Context\FooStub', $this->container->make('Titon\Test\Stub\Context\FooStub'));
    }

    public function testRegisterInstance(): void {
        $definition = $this->container->register('foo', new FooStub());

        $this->assertInstanceOf('Titon\Context\Definition\ObjectDefinition', $definition);
        $this->assertInstanceOf('Titon\Test\Stub\Context\FooStub', $this->container->make('foo'));
        $this->assertTrue($this->container->isSingleton('foo'));
        $this->assertSame($this->container->make('foo'), $this->container->make('Titon\Test\Stub\Context\FooStub'));
        $this->assertSame($this->container->make('foo'), $definition->get());
    }

    public function testRegisterClosure(): void {
        $this->container->register('foo', () ==> new FooStub('Foo'));

        $this->assertInstanceOf('Titon\Test\Stub\Context\FooStub', $this->container->make('foo'));
        $this->assertEquals('Foo', $this->container->make('foo')->getName());
    }

    public function testRegisterCallable(): void {
        $this->container->register('foo', ['Titon\Test\Stub\Context\FooStub', 'factory']);
        $this->assertInstanceOf('Titon\Test\Stub\Context\FooStub', $this->container->make('foo'));

        $this->container->register('Foo', 'Titon\Test\Stub\Context\FooStub::factory');
        $this->assertInstanceOf('Titon\Test\Stub\Context\FooStub', $this->container->make('Foo'));
    }

    public function testAutoDependencyResolution(): void {
        $test = $this->container->make('Titon\Test\Stub\Context\BarStub');
        $this->assertInstanceOf('Titon\Test\Stub\Context\BarStub', $test);
        $this->assertInstanceOf('Titon\Test\Stub\Context\FooStub', $test->getFoo());
    }

    public function testSingletonNestedResolution(): void {
        $this->container->singleton('foo', 'Titon\Test\Stub\Context\FooStub');

        $foo = $this->container->make('foo');
        $bar = $this->container->make('Titon\Test\Stub\Context\BarStub');

        $this->assertSame($foo, $bar->getFoo());
        $this->assertSame($foo, $this->container->make('Titon\Test\Stub\Context\FooStub'));
    }

    public function testMethodInjection(): void {
        $this->container->register('foo', 'Titon\Test\Stub\Context\FooStub')->call('setName', 'Foo Bar');
        $this->assertInstanceOf('Titon\Test\Stub\Context\FooStub', $this->container->make('foo'));
        $this->assertEquals('Foo Bar', $this->container->make('foo')->getName());

        $this->container->remove('foo');

        $this->container->register('foo', 'Titon\Test\Stub\Context\FooStub')->call('setBar', new BarStub(new FooStub('Alex Phillips')));
        $this->assertInstanceOf('Titon\Test\Stub\Context\FooStub', $this->container->make('foo'));
        $this->assertEquals('Alex Phillips', $this->container->make('foo')->getName());
    }

    public function testMethodChaining(): void {
        $test = $this->container->make('Titon\Test\Stub\Context\BarStub')->getFoo();
        $this->assertInstanceOf('Titon\Test\Stub\Context\FooStub', $test);
    }

    public function testConstructorInjection(): void {
        $this->container->register('bar', 'Titon\Test\Stub\Context\BarStub')->with('Titon\Test\Stub\Context\FooStub');
        $this->assertInstanceOf('Titon\Test\Stub\Context\BarStub', $this->container->make('bar'));

        $this->container->remove('bar');

        $this->container->register('bar', 'Titon\Test\Stub\Context\BarStub')->with((new FooStub())->setName('Foo Bar'));
        $this->assertInstanceOf('Titon\Test\Stub\Context\BarStub', $this->container->make('bar'));
        $this->assertEquals($this->container->make('bar')->getFoo()->getName(), 'Foo Bar');
    }

    public function testPassArgumentsAtMake(): void {
        $this->container->register('bar', 'Titon\Test\Stub\Context\BarStub');
        $test = $this->container->make('bar', new FooStub('Foo Bar'));
        $this->assertInstanceOf('Titon\Test\Stub\Context\BarStub', $test);
        $this->assertEquals('Foo Bar', $test->getFoo()->getName());
    }

    public function testAliasing(): void {
        $this->container->register('foo', 'Titon\Test\Stub\Context\FooStub', true);
        $this->container->alias('foobar', 'Titon\Test\Stub\Context\FooStub');

        $this->assertSame($this->container->make('foo'), $this->container->make('foobar'));
        $this->assertSame($this->container->make('foobar'), $this->container->make('Titon\Test\Stub\Context\FooStub'));
    }

    public function testAliasChaining(): void {
        $this->container->register('Titon\Test\Stub\Context\FooStub')->alias('foo');
        $this->assertInstanceOf('Titon\Test\Stub\Context\FooStub', $this->container->make('Titon\Test\Stub\Context\FooStub'));
        $this->assertInstanceOf('Titon\Test\Stub\Context\FooStub', $this->container->make('foo'));
    }

    public function testNestedDependencyResolution(): void {
        $test = $this->container->make('Titon\Test\Stub\Context\BarStub');
        $this->assertEquals('Alex Phillips', $test->getFoo()->getName());
    }

    public function testCallableResolution(): void {
        $this->assertEquals('Alex Phillips', $this->container->make('Titon\Test\Stub\Context\foo_stub_func'));
    }

    public function testClosureDependencyResolution(): void {
        $bar = $this->container->make((FooStub $foo) ==> new BarStub($foo));

        $this->assertEquals('Alex Phillips', $bar->getFoo()->getName());
    }

    public function testMakeSingleton(): void {
        $this->container->register('Titon\Test\Stub\Context\FooStub');
        $this->assertNotSame($this->container->make('Titon\Test\Stub\Context\FooStub'), $this->container->make('Titon\Test\Stub\Context\FooStub'));

        $this->container->makeSingleton('Titon\Test\Stub\Context\FooStub');
        $this->assertSame($this->container->make('Titon\Test\Stub\Context\FooStub'), $this->container->make('Titon\Test\Stub\Context\FooStub'));
    }

    public function testServiceProviders(): void {
        $this->container->addServiceProvider('Titon\Test\Stub\Context\FooServiceProviderStub');
        $this->assertEquals(new FooStub(), $this->container->make('Titon\Test\Stub\Context\FooStub'));
        $this->assertEquals(new FooStub(), $this->container->make('foo'));

        $this->container->addServiceProvider('Titon\Test\Stub\Context\BarServiceProviderStub');
        $this->assertSame($this->container->make('bar'), $this->container->make('Titon\Test\Stub\Context\BarStub'));
    }

}
