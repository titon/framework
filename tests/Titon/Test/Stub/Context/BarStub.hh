<?hh // strict
namespace Titon\Test\Stub\Context;

class BarStub {
    protected FooStub $foo;

    public function __construct(FooStub $foo) {
        $this->foo = $foo;
    }

    public function getFoo(): FooStub {
        return $this->foo;
    }

    public function setFoo(FooStub $foo): this {
        $this->foo = $foo;

        return $this;
    }
}
