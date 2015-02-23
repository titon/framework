# Injection #

Injection allows the user to inject arguments into the constructor of the registered item or to have the item automatically call methods after instantiation.

## Constructor Injection ##

Constructor injection allows the user to inject specific arguments into the constructor. Using our examples from the [container docs](container.md), lets set a constructor argument for `Foo`.

```hack
$container->register('foo', 'Foo')
    ->with(new Bar('Alex Phillips'));
```

We can take this a step further by passing an alias to with, which will attempt to pass another registered dependency into the constructor.

```hack
$container->register('bar', 'Bar')
$container->register('foo', 'Foo')
    ->with('bar');
```

Passing multiple arguments is as simple as declaring multiple arguments on `with()`.

```hack
$container->register('foo', 'Foo')
    ->with('bar', 'baz');
```

## Method Injection ##

Method injection allows the user to automatically call methods immediately after instantiating a registered item. This is only applicable for items registered as classes.

```hack
$container->register('foo', 'Foo')
    ->call('setBar', new Bar('Alex Phillips'));
```
