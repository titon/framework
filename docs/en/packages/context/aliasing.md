# Aliasing #

Aliasing allows the user to define an additional key retrieve a registered item by.

```hack
$container->register('Titon\Foo');
$container->alias('foo', 'Titon\Foo');

// Optionally set initial alias
$container->make('foo');
```

Fluent method chaining

```hack
$container->register('Titon\Foo')->alias('foo');
```

If, at any point, an existing alias is registered, a `Titon\Context\Exception\AlreadyRegisteredException' is thrown.