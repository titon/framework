# Aliasing #

Aliasing allows the user to define an additional key retrieve a registered item by. Adding an alias to a class name will
generate the same return value as calling `make` on the originally registered item.

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

If an item is registered as a singleton an an alias is added, calling `make` on the alias will return the same singleton
instance.

```hack
$container->register('Titon\Foo');
$container->alias('foo', 'Titon\Foo');

// Optionally set initial alias
$container->make('foo');
```

If, at any point, an existing alias is registered, a `Titon\Context\Exception\AlreadyRegisteredException` is thrown.