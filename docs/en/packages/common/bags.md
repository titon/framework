# Bags #

The `Titon\Common\Bag` interface provides an easy object oriented approach for managing a set of parameters. An abstract class exists at `Titon\Common\Bag\AbstractBag` that can be inherited for immediate functionality. This class inherits most of its functionality from the [Titon\Common\Mutable](traits.md) trait.

```hack
class ExampleBag extends Titon\Common\Bag\AbstractBag<string, array<string>> {
    // ...
}

$bag = new ExampleBag();
```

<div class="notice is-info">
    The <code>Mutable</code> trait supports dot-notated keys.
</div>

<div class="notice is-warning">
    When inheriting from <code>AbstractBag</code>, they key-value generics must be defined.
</div>

## Adding Parameters ##

There are 2 methods for adding parameters to a bag, the first with `set()`, which accepts a key and value. Using the example above, our key must be a `string` and our value must be an `array<string>`.

```hack
$bag->set('foo', ['bar']);
$bag->set('baz', 'qux'); // Invalid
```

The second method is `add()`, which accepts a map of key-value pairs.

```hack
$bag->set(Map {
    'foo' => ['bar'],
    'baz' => ['qux']
});
```

## Retrieving Parameters ##

To retrieve a single parameter, use the `get()` method.

```hack
$bag->get('foo');
$bag->get('bar.baz'); // Nested
```

Or to retrieve all parameters, use the `all()` method.

```hack
$bag->all(); // Map<Tk, Tv>
```

## Removing Parameters ##

The `remove()` method can be used for deleting a parameter defined by key.

```hack
$bag->remove('foo');
```

And the `flush()` method will delete all parameters.

```hack
$bag->flush();
```

## Checking Parameters ##

Use the `has()` method to check whether a parameter exists by key.

```hack
$bag->has('foo');
```
