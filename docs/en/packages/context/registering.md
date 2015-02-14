# Registering #

## Registering a class ##

Passing in a class name will add that class to the depository's registry for retrieval later. Note: classes are not resolved
until `make` is called to retrieve the object.

```hack
$container->register('Titon\Foo');

// Optionally set initial alias
$container->register('foo', 'Titon\Foo');
```

Passing in a class instance will register the the class as a singleton. This causes every `make` call on that key to return
the same instance.

```hack
$container->register('foo', new Foo());

// Both will resolve to the same object reference
$container->make('Titon\Foo');
$container->make('foo');
```

## Registering a Closure ##

Registering a Closure will match the provided key to a Closure object. The value from a `make` call will be the returned
value from the Closure.

```hack
$container->register('foo', function() {
    $foo = new Foo('bar');
    
    return $foo;
});

$container->make('foo'); // Returned 'Foo' instance with 'bar' passed in during construction
```

## Registering a class by callable ##

Passing in a `callable` will register the callable to the `key` parameter provided. When `make` is run on the callable,
the value returned is the return value of the callable itself.

```hack
$container->register('foo', 'Foo');
$container->make('foo'); // Returns 'bar'

$container->register('baz', array('Bar', 'Baz'));
$container->make('baz'); // Returns 'foo'

function Foo() {
    return 'bar';
}

class Bar
{
    public static function Baz()
    {
        return 'foo';
    }
}
```

## Registering a singleton ##

Singletons may be registered by either specifying a singleton through the `boolean` parameter in the register method or
by the `singleton` method of the depository object. Also, calling `register` with an instance of an object will automatically
add that instance as a singleton.

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

## ArrayAccess ##

ArrayAccess is also supported for registering items as well as making items. Note: all items set through ArrayAccess are
NOT set as singletons unless the set value is an object instance.

```hack
$container['foo'] = 'Titon\Foo';
$foo = $container['foo'];

// Singleton registered
$container['foo'] = new Foo();
```

If, at any point, an existing key is registered, a `Titon\Context\Exception\AlreadyRegisteredException' is thrown.