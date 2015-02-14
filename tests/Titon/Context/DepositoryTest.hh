<?hh
/**
 * @author Alex Phillips <exonintrendo@gmail.com>
 * Date: 2/5/15
 * Time: 6:05 PM
 */

namespace Titon\Context;

use Titon\Test\TestCase;

/**
 * @property \Titon\Context\Depository $object
 */
class DepositoryTest extends TestCase
{
    protected function setUp()
    {
        parent::setUp();

        $this->object = new Depository();
    }

    public function testRegisterClassName()
    {
        $this->object->register('foo', 'Titon\\Context\\Foo');
        $this->assertInstanceOf('Titon\\Context\\Foo', $this->object['foo']);
    }

    public function testClosure()
    {
        $this->object->register('bar', function() {
            $test = new Bar((new Foo())->setName('Foo Bar'));

            return $test;
        });

        $this->assertInstanceOf('Titon\\Context\\Bar', $this->object['bar']);
        $this->assertEquals('Foo Bar', $this->object['bar']->getFoo()->getName());
    }

    public function testSingleton()
    {
        $test = new Foo();
        $this->object->singleton('foo', $test);

        $this->assertEquals('Alex Phillips', $this->object['foo']->getName());

        $test = new Bar((new Foo())->setName('Foo Bar'));
        $this->object->singleton('bar', $test);

        $test = $this->object['bar'];
        $this->assertInstanceOf('Titon\\Context\\Bar', $test);
        $this->assertEquals('Foo Bar', $test->getFoo()->getName());
    }

    public function testAutoDependencyResolution()
    {
        $test = $this->object['Titon\\Context\\Bar'];
        $this->assertInstanceOf('Titon\\Context\\Bar', $test);
        $this->assertInstanceOf('Titon\\Context\\Foo', $test->getFoo());
    }

    public function testClassDefinitionWithMethodCalls()
    {
        $this->object->register('foo', 'Titon\\Context\\Foo')->call('setName', 'Foo Bar');
        $this->assertInstanceOf('Titon\\Context\\Foo', $this->object['foo']);
        $this->assertEquals('Foo Bar', $this->object['foo']->getName());
    }

    public function testCallMethodsWithObjects()
    {
        $test = $this->object['Titon\\Context\\Bar']->getFoo();
        $this->assertInstanceOf('Titon\\Context\\Foo', $test);
    }

    public function testConstructorInjection()
    {
        $this->object->register('bar', 'Titon\\Context\\Bar')->with('Titon\\Context\\Foo');
        $this->assertInstanceOf('Titon\\Context\\Bar', $this->object['bar']);

        unset($this->object['bar']);

        $this->object->register('bar', 'Titon\\Context\\Bar')->with((new Foo())->setName('Foo Bar'));
        $this->assertInstanceOf('Titon\\Context\\Bar', $this->object['bar']);
        $this->assertEquals($this->object['bar']->getFoo()->getName(), 'Foo Bar');
    }

    public function testMethodInjection()
    {
        $this->object->register('foo', 'Titon\\Context\\Foo')->call('setName', 'Foo Bar');
        $this->assertEquals('Foo Bar', $this->object['foo']->getName());
    }

    public function testArgumentsAtCreation()
    {
        $this->object->register('bar', 'Titon\\Context\\Bar');
        $test = $this->object->make('bar', new Foo('Foo Bar'));
        $this->assertInstanceOf('Titon\\Context\\Bar', $test);
        $this->assertEquals('Foo Bar', $test->getFoo()->getName());
    }

    public function testAliasing()
    {
        $this->object->register('foo', 'Titon\\Context\\Foo', true);
        $this->object->alias('foobar', 'Titon\\Context\\Foo');

        $this->assertSame($this->object->make('foo'), $this->object->make('foobar'));
    }

    public function testAliasChaining()
    {
        $this->object->register('Titon\\Context\\Foo')->alias('foo');
        $this->assertInstanceOf('Titon\\Context\\Foo', $this->object->make('Titon\\Context\\Foo'));
        $this->assertInstanceOf('Titon\\Context\\Foo', $this->object->make('foo'));

        $this->object->alias('bar', 'Titon\\Context\\Bar');
        $this->assertInstanceOf('Titon\\Context\\Bar', $this->object->make('Titon\\Context\\Bar'));
        $this->assertInstanceOf('Titon\\Context\\Bar', $this->object->make('bar'));
    }
}

class Foo
{
    public $name;

    public function __construct($name = 'Alex Phillips')
    {
        $this->name = $name;
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