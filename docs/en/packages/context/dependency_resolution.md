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