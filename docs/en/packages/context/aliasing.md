# Aliasing #

Aliasing allows the user to define an additional key to retrieve a registered item by. Adding an alias to a class name will generate the same return value as calling `make()` on the originally registered item.

```hack
$container = new Titon\Context\Depository();
$container->register('Foo');
$container->alias('foo', 'Foo');
$container->make('foo'); // Foo
```

Fluent method chaining is available when registering an item.

```hack
$container->register('Foo')->alias('foo');
```

If an item is registered as a singleton and an alias is added, calling `make()` on the alias will return the same singleton instance.

```hack
$container->register('Foo');
$container->alias('foo', 'Foo');

// Optionally set initial alias
$container->make('foo');
```

If, at any point, an existing alias is registered, a `Titon\Context\Exception\AlreadyRegisteredException` is thrown.
