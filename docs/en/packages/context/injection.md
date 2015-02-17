# Injection #

Injection allows the user to injection arguments into the constructor of the registered item or to have the item
automatically call methods after instantiation.

## Constructor Injection ##

Constructor injection allows the user to inject specific arguments into the constructor.

```hack
$container->register('foo', 'Foo')->with(new Bar('Alex Phillips'));

$foo = $container->make('foo');

class Foo()
{
    private $bar;

    public function __construct(Bar $bar)
    {
        $this->bar = $bar;
    }
}

class Bar
{
    private $name;

    public function __construct($name)
    {
        $this->name = $name;
    }
}
```

## Method Injection

Method injection allows the user to automatically call methods immediately after instantiating a registered item.

```hack
$container->register('foo', 'Foo')->call('setBar', new Bar('Alex Phillips'));

$foo = $container->make('foo');

class Foo()
{
    private $bar;

    public function __construct(Bar $bar = null)
    {
        $this->bar = $bar;
    }

    public function setBar(Bar $bar)
    {
        $this->bar = $bar;
    }
}

class Bar
{
    private $name;

    public function __construct($name)
    {
        $this->name = $name;
    }
}
```