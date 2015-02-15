# Auto Dependency Resolution #

After registering an item, if the constructor requires arguments that haven't been specified through injection, the
depository will construct the necessary dependencies based on type hints and default values. This will account for
nested dependencies as well.

```hack
$container->register('foo', 'Foo');

$foo = $container->make('foo')->getBar()->getName(); // Alex Phillips

class Foo()
{
    private $bar;

    public function __construct(Bar $bar)
    {
        $this->bar = $bar;
    }

    public function getBar()
    {
        return $this->bar;
    }
}

class Bar
{
    private $name;

    public function __construct($name = 'Alex Phillips')
    {
        $this->name = $name;
    }

    public function getName()
    {
        return $this->name;
    }
}
```

## Non-registered Items ##

You can also us the depository to resolve classes and callables that have not been registered and the depository will auto-resolve any necessary dependencies.

```hack
$container->make('Foo');

class Foo
{
    private $name;

    public function __construct($name = 'Alex Phillips')
    {
        $this->name = $name;
    }
}
```

This will work on Closures as well:

```hack
$$bar = $container->make(function(Foo $foo) {
    return $foo->getBar();
});

class Foo()
{
    private $bar;

    public function __construct(Bar $bar)
    {
        $this->bar = $bar;
    }

    public function getBar()
    {
        return $this->bar;
    }
}

class Bar
{
    private $name;

    public function __construct($name = 'Alex Phillips')
    {
        $this->name = $name;
    }

    public function getName()
    {
        return $this->name;
    }
}
```
