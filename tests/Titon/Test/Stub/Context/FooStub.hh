<?hh // strict
namespace Titon\Test\Stub\Context;

class FooStub
{
    public $name;

    private $bar;

    public function __construct($name = 'Alex Phillips')
    {
        $this->name = $name;
    }

    public static function factory($name = 'Alex Phillips')
    {
        return new FooStub($name);
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

    public function setBar(BarStub $bar)
    {
        $this->bar = $bar;
    }
}

function foo_stub_func(FooStub $foo): void {
    return $foo->getName();
}
