# Registering #

Registering a class by name

```hack
$container->register('Titon\Foo');

// Optionally set initial alias
$container->register('foo', 'Titon\Foo');
```

Registering a class by Closure

```hack
$container->register('foo', function() {
    $foo = new Foo('bar');
    
    return $foo;
});
```

Registering a class by callable

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

Singletons may be registered by either specifying a singleton through the `boolean` parameter in the register method or
by the `singleton` method of the depository object.

```hack
$container->register('Titon\Foo', null, true);

// with an alias
$container->register('foo', 'Titon\Foo', true);

$container->singleton('Titon\Bar');

// with an alias
$container->singleton('bar', 'Titon\Bar');
```

ArrayAccess is also supported for registering items as well as making items. Note, all items set through ArrayAccess are
NOT set as singletons.

```hack
$container['foo'] = 'Titon\Foo';
$foo = $container['foo'];
```

If, at any point, an existing key is registered, a `Titon\Context\Exception\AlreadyRegisteredException' is thrown.