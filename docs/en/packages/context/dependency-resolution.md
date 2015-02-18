# Auto-Dependency Resolution #

After registering an item, if the constructor requires arguments that haven't been specified through injection, the depository will construct the necessary dependencies based on type hints and default values. This will account for nested dependencies as well.

```hack
$container->register('foo', 'Foo');

$foo = $container->make('foo')->getBar()->getName(); // Alex Phillips

class Foo {
    private Bar $bar;

    public function __construct(Bar $bar) {
        $this->bar = $bar;
    }

    public function getBar(): Bar {
        return $this->bar;
    }
}

class Bar {
    private string $name;

    public function __construct(string $name = 'Alex Phillips') {
        $this->name = $name;
    }

    public function getName(): string {
        return $this->name;
    }
}
```

## Non-registered Items ##

You can also use the depository to resolve classes and callables that have not been registered. The depository will also auto-resolve any necessary dependencies.

```hack
$container->make('Foo');

class Foo {
    private string $name;

    public function __construct(string $name = 'Alex Phillips') {
        $this->name = $name;
    }
}
```

Resolving closures are done by calling `run` on the depository.

```hack
$bar = $container->call(function(Foo $foo) {
    return $foo->getBar();
});

class Foo {
    private Bar $bar;

    public function __construct(Bar $bar) {
        $this->bar = $bar;
    }

    public function getBar(): Bar {
        return $this->bar;
    }
}

class Bar {
    private string $name;

    public function __construct(string $name = 'Alex Phillips') {
        $this->name = $name;
    }

    public function getName(): string {
        return $this->name;
    }
}
```
