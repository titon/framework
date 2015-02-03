# Subjects #

A subject is an interface that provides event management support within individual classes. The `Titon\Event\Subject` 
interface defines a set of methods for emitting events and subscribing observers. To make development easier, 
the `Titon\Event\Emittable` trait can be used that will implement `Subject` out of the box.

```hack
class Foo implements Titon\Event\Subject {
    use Titon\Event\Emittable;
}

$foo = new Foo();
```

## Managing Observers ##

[Observers](emitting.md#observers) can be subscribed using the `on()` method. 
This method accepts either a callable or a [listener](listeners.md).

```hack
$foo->on('init', $callback);
$foo->on(new ExampleListener());
```

Observers can also be [flagged as once](emitting.md#one-time-notifications) using the `once()` method. 
This method does not support [listeners](listeners.md).

```hack
$foo->once('init', $callback);
```

And to remove an observer or [listener](listeners.md), use `off()`.

```hack
$foo->off('init', $callback);
$foo->off($listener);
```

## Dispatching Events ##

The `emit()` and `emitMany()` methods can be used for dispatching events. Information on these methods and usage can be 
found in the [event dispatching documentation](emitting.md#dispatching-events).

```hack
$event = $foo->emit('init');
$events = $foo->emitMany(['saved', 'deleted']);
```

To use a custom `Titon\Event\Event` object, the `dispatch()` method on the internal `Titon\Event\Emitter` must be used.

```hack
$event = $foo->getEmitter()->dispatch(new ExampleEvent('updated'));
```

## Custom Emitter ##

A custom [emitter](emitting.md) can be set using `setEmitter()`. This should be done before any events are dispatched.

```hack
$foo->setEmitter(new ExampleEmitter());
```
