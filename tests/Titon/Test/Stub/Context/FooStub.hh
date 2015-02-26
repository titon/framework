<?hh // strict
namespace Titon\Test\Stub\Context;

class FooStub {
    public string $name;

    protected ?BarStub $bar;

    public function __construct(string $name = 'Alex Phillips') {
        $this->name = $name;
    }

    public static function factory(string $name = 'Alex Phillips'): FooStub {
        return new FooStub($name);
    }

    public function getName(): string {
        return $this->name;
    }

    public function setName(string $name): this {
        $this->name = $name;

        return $this;
    }

    public function setBar(BarStub $bar): this {
        $this->bar = $bar;

        return $this;
    }
}

function foo_stub_func(FooStub $foo): string {
    return $foo->getName();
}
