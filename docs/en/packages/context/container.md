# Containers #

A container implements the Inversion of Control (IoC) pattern by managing a collection of dependencies that is easily accessible by external callers. The container also provides dependency lazy-loading, item aliasing, constructor and method injection, and auto-resolution. A container can be instantiated using the `Titon\Context\Depository` class.

```hack
$container = new Titon\Context\Depository();
```

<div class="notice is-info">
    Container is a reserved word in Hack/HHVM, so we opted to go with a unique class name that is easily recognizable.
</div>

## Registering A Class ##

Passing in a class name to `register()` will add that class to the depository's registry for retrieval later.

```hack
$container->register('Foo');
$container->register('foo', 'Foo'); // With an alias
```

<div class="notice is-info">
    Classes are not resolved until <code>make()</code> is called to retrieve the object.
</div>

Passing in a class instance will register the class as a singleton. This causes every `make()` call to return the same instance.

```hack
$container->register('foo', new Foo());

// Both will resolve to the same object reference
$container->make('Foo');
$container->make('foo');
```

## Registering A Closure ##

Registering a closure will match the provided key to a closure object. The `make()` call will return the value from the closure.

```hack
$container->register('foo', function() {
    return new Foo('bar');
});

$container->make('foo'); // Returned 'Foo' instance with 'bar' passed in during construction
```

## Registering A Class By Callable ##

Passing in a `callable` will register the callable to the `key` parameter provided. The `make()` call will return the value from the callable.

```hack
$container->register('foo', 'Foo');
$container->make('foo'); // Returns 'bar'

$container->register('baz', ['Bar', 'staticMethod']);
$container->make('baz'); // Returns 'foo'

function Foo(): string {
    return 'bar';
}

class Bar {
    public static function staticMethod(): string {
        return 'foo';
    }
}
```

## Registering A Singleton ##

Singletons may be registered by either specifying true as the 3rd parameter in the `register()` method or by using the `singleton()` method on the depository object. Also, calling `register()` with an instance of an object will automatically add that instance as a singleton.

```hack
$container->register('Titon\Foo', null, true);

// with an alias
$container->register('foo', 'Titon\Foo', true);

$container->singleton('Titon\Bar');

// with an alias
$container->singleton('bar', 'Titon\Bar');

// Register a singleton using an instance
$container->register(new Foo());
```

If, at any point, an existing key is registered, a `Titon\Context\Exception\AlreadyRegisteredException` is thrown.
