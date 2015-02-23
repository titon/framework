# Auto-Dependency Resolution #

After registering an item, if the constructor requires arguments that haven't been specified through injection, the depository will construct the necessary dependencies based on type hints and default values. This will account for nested dependencies as well.

Let's add to our [previous examples](container.md) by adding a new `Baz` class.

```hack
class Baz {
    protected Foo $foo;
    protected Bar $bar;
    
    public function __construct(Foo $foo, Bar $bar) {
        $this->foo = $foo;
        $this->bar = $bar;
    }
}

$container->register('Baz');
$baz = $container->make('Baz'); // Will automatically set `Foo` and `Bar`
```

## Non-Registered Items ##

You can also use the depository to resolve classes and callables that have not been registered, by simply passing the class name to `make()`. The depository will also attempt to auto-resolve any necessary dependencies.

```hack
class Qux {}

$qux = $container->make('Qux');
```

Resolving arguments within closures can be accomplished by calling `call()` on the depository.

```hack
$bar = $container->call(function(Foo $foo) {
    return $foo->getBar();
});
```
