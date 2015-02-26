<?hh // strict
namespace Titon\Test\Stub\Context;

class BarStub
{
    private $foo;

    public function __construct(FooStub $foo)
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
