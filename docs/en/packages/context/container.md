# Containers #

A container implements the Inversion of Control (IoC) pattern by managing a collection of dependencies that is easily accessible by external callers. The container also provides dependency lazy-loading, item aliasing, constructor and method injection, and dependency auto-resolution. A container can be instantiated using the `Titon\Context\Depository` class.

```hack
$container = new Titon\Context\Depository();
```

<div class="notice is-info">
    Container is a reserved word in Hack/HHVM, so we opted to go with a unique class name that is easily recognizable.
</div>

## Registering ##

Containers manage dependencies through a registration layer that maps unique keys (or aliases) to a class or callable. All forms of registration are handled with the `register()` method. Before we continue, let's create a few example classes and functions that we will use throughout the documentation.

```hack
class Foo {
    protected ?Bar $bar;

    public function __construct(?Bar $bar = null) {
        $this->bar = $bar;
    }

    public function getBar(): Bar {
        return $this->bar;
    }
}

class Bar {
    protected string $name;

    public function __construct(string $name = 'Alex Phillips') {
        $this->name = $name;
    }

    public function getName(): string {
        return $this->name;
    }
    
    public static function factory(): Bar {
        return new static();
    }
}

function baz(): string {
    return 'baz';
}
```

### A Class ###

Registering a fully qualified class as a string will store a lazy-loaded reference to the dependency. The class will not be resolved until `make()` is called to retrieve the object. 

```hack
$container->register('Foo');
$container->register('foo', 'Foo'); // With an alias
```

Registering a class instance directly will register the object as a singleton. This causes every `make()` call to return the same instance.

```hack
$container->register('foo', new Foo());

// Both will resolve to the same object reference
$foo = $container->make('Foo');
$foo = $container->make('foo');
```

### A Callable ###

Registering a closure acts as a form of a factory, in which the closure returns the instance of another object. The return value of the closure will be returned when `make()` is called.

```hack
$container->register('foo', function() {
    return new Foo('bar');
});

$foo = $container->make('foo'); // Returned a `Foo` instance
```

Callables, static methods, and global functions are also supported.

```hack
$container->register('baz', 'baz');
$container->register('bar', ['Bar', 'factory']);
$container->register('bar', 'Bar::factory');
```

### A Singleton ###

Singletons may be registered by either specifying true as the 3rd parameter in the `register()` method or by using the `singleton()` method on the depository object. Also mentioned previously, calling `register()` with an instance of an object will automatically add that instance as a singleton.

```hack
$container->register('Foo', '', true);
$container->register('foo', 'Foo', true); // Aliased

$container->singleton('Bar');
$container->singleton('bar', 'Bar'); // Aliased

$container->register(new Foo()); // Instances are stored as singletons
```

If, at any point, an existing key is registered, a `Titon\Context\Exception\AlreadyRegisteredException` is thrown.

## Aliasing ##

Aliasing allows the user to define an additional key to retrieve a registered item by. Adding an alias to a class name will generate the same return value when calling `make()` on the originally registered item. Furthermore, if an item is registered as a singleton and an alias is added, calling `make()` on the alias will return the same singleton instance.

```hack
$container->register('Foo');
$container->alias('foo', 'Foo');

$foo = $container->make('foo'); // Foo
```

Fluent method chaining is also available when registering an item.

```hack
$container->register('Foo')->alias('foo');
```

If, at any point, an existing alias is registered, a `Titon\Context\Exception\AlreadyRegisteredException` is thrown.

## Making ##

To instantiate a dependency, we can use the `make()` method. This method will attempt to instantiate an item, resolve any nested dependencies, and return an object. Usage of this method has been used throughout the previous examples.

```hack
$foo = $container->make('Foo');
```

The method accepts either an alias, or the class name.
