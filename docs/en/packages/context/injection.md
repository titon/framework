# Injection #

Injection allows the user to inject arguments into the constructor of the registered item or to have the item automatically call methods after instantiation.

## Constructor Injection ##

Constructor injection allows the user to inject specific arguments into the constructor.

```hack
$container->register('foo', 'Foo')->with(new Bar('Alex Phillips'));

$foo = $container->make('foo');

class Foo {
    private Bar $bar;

    public function __construct(Bar $bar) {
        $this->bar = $bar;
    }
}

class Bar {
    private string $name;

    public function __construct(string $name) {
        $this->name = $name;
    }
}
```

## Method Injection ##

Method injection allows the user to automatically call methods immediately after instantiating a registered item.

```hack
$container->register('foo', 'Foo')->call('setBar', new Bar('Alex Phillips'));

$foo = $container->make('foo');

class Foo {
    private ?Bar $bar;

    public function __construct(?Bar $bar = null) {
        $this->bar = $bar;
    }

    public function setBar(Bar $bar): void {
        $this->bar = $bar;
    }
}

class Bar {
    private string $name;

    public function __construct($name) {
        $this->name = $name;
    }
}
```
