# Event Dispatching #

Event driven programming is a paradigm in which certain parts of an application are notified when a specific action occurs. 
The `Titon\Event\Emitter` class handles this scenario by managing a list of observers to notify about an event (the action), 
and the actual process of emitting the event (also known as dispatching).

Every instance of `Emitter` manages their own unique list of observers and events. To provide a global event dispatching 
system, a single instance of the `Emitter` should be used, either through a singleton, or a container.

```hack
$emitter = new Titon\Event\Emitter();
```

Emitters can also be used on the class level by utilizing [subjects](subjects.md).

## Events ##

An event is referenced as one of two things -- either an instance of `Titon\Event\Event` with its own unique name, 
or the name of a key used for dispatching. The key will always correlate to an event name.

Both variations of the event will be used interchangeably in the documentation going forward, 
so be weary of this while reading.

## Observers ##

An observer in the context of this package is a callable that subscribes to an event
and is notified when that event is dispatched. There are multiple forms that an observer can take -- 
either with a closure/lambda, a callable via `inst_meth()` or `class_meth()`, or through a [listener](listeners.md).

An observer's declaration must conform to the `Titon\Event\ObserverCallback` [type alias](types.md), 
with the 1st argument always being an instance of `Titon\Event\Event`.

For a better understanding of observer declarations, here are a few examples. 
The first, with in lambda form.

```hack
$observer = ($event) ==> {
    // ...
};
```

And now a with class method using `inst_meth()`.

```hack
class ExampleClass {
    public function foo(Event $event): mixed {
        // ...
    }
}

$class = new ExampleClass();
$observer = inst_meth($class, 'foo');
```

And finally, with a listener. [Jump to the listeners docs for more information](listeners.md).

```hack
class ExampleListener implements Titon\Event\Listener {
    public function foo(Event $event): mixed {
        // ...
    }
    
    public function subscribeToEvents(): Titon\Event\ListenerMap {
        return Map {'init' => 'foo'};
    }
}
```

### Asynchronous Observers ###

The event package supports both synchronous (default) and asynchronous observers. When an event is dispatched, 
all non-async observers will be notified in [order of priority](#priority-queue), which allows for critical 
must-run-first observers to always be notified. Once complete, all remaining async observers will be notified in parallel.

To make use of async observers, simply add the `async` modifier and the `Awaitable<mixed>` return type to the method.

```hack
class ExampleClass {
    public async function foo(Event $event): Awaitable<mixed> {
        // ...
    }
}
```

## Subscribing To Events ##

Now that we have a better understanding of observers, we can now subscribe observers to an event, via a unique key. 
This can be accomplished using the `subscribe()` and `listen()` methods. The `subscribe()` method 
requires a unique event key and a callable.

```hack
$emitter->subscribe('init', ($event) ==> {});
$emitter->subscribe('load', inst_meth($class, 'method'));
```

The `listen()` method on the other hand simply requires an instance of an object that implements the 
`Titon\Event\Listener` interface.

```hack
$emitter->listen(new ExampleListener());
```

### Priority Queue ###

Observers aren't notified in the order that they are defined, but in the order of priority. Priority can be defined 
by passing an integer to the 3rd argument of `subscribe()`. If the priority is omitted, the priority will default 
to 100 + the number of current observers. A priority level below 100 is reserved for internal and 
critical observers that must be notified before all others.

```hack
$emitter->subscribe('init', $callback, 55);
```

Pass the constant `Emitter::AUTO_PRIORITY` for automatic priorities. 

### One-Time Notifications ###

It's possible for observers to be notified multiple times if the event in question is dispatched multiple times. 
To avoid executing the observer more than once, a boolean true can be set as the 4th argument. 

```hack
$emitter->subscribe('init', $callback, Emitter::AUTO_PRIORITY, true);
```

### Unsubscribing ###

To remove an observer from an event, the original callable must be passed to `unsubscribe()`, or the listener object to 
`unlisten()`.

```hack
$emitter->unsubscribe('init', $callback);
$emitter->unlisten($listener);
```

## Dispatching Events ##

Once subscriptions are in place, all that's left to do is notify them, which can be done using the `emit()` method. 
The `emit()` method accepts a unique key, which is the name of the event to dispatch, and an optional array of arguments 
to pass to each observer.

```hack
$event = $emitter->emit('init');
```

When arguments are defined, they can be accessed as arguments within the observer.

```hack
$observer = ($event, $foo, $bar) ==> {
    // ...
};

$event = $emitter->emit('init', [$foo, $bar]);
```

Arguments also support passing by reference.

```hack
$observer = ($event, $foo, &$bar) ==> {
    // ...
};

$event = $emitter->emit('init', [$foo, &$bar]);
```

The response of this method will be an instance of `Titon\Event\Event` -- the same instance passed to each observer.

### Multiple Dispatch ###

Multiple events can be dispatched using `emitMany()` and providing an array of event keys, 
or a string of space separated event keys.

```hack
$events = $emitter->emitMany('foo bar', $params);
$events = $emitter->emitMany(['foo', 'bar'], $params);
```

When using this approach, each unique key will create a new `Event` object, but any arguments will be shared.

The response of this method will be a mapping of `Event` objects by unique key, instead of a single object.

#### Wildcard Events ####

To make multiple dispatching easier, a wildcard can be used using `*`. This wildcard will match any alpha-numeric 
character, a dash, or underscore.

```hack
$events = $emitter->emitMany('foo.*', $params);
```

The previous example will notify any event that begins with `foo.`, like `foo.bar`, `foo.baz`, etc.

### Custom Event Objects ###

Every time `emit()` is called, a `Titon\Event\Event` object is automatically created. To use a custom `Event` object, 
the `dispatch()` method can be used.

```hack
$event = $emitter->dispatch(new ExampleEvent('saved'), $params);
```

## Persisting Data ##

Data can be persisted between observers by setting data with `setData()` and retrieving it with `getData()`. 
Both methods support dot notated keys.

```hack
$observer = ($event) ==> {
    $event->setData('foo', 'bar');
};

$observer = ($event) ==> {
    $foo = $event->getData('foo');
};
```

Do note that the priority of the observer will determine the availability of data, as observers are notified in order,
excluding async observers, which are notified in parallel.

## Interrupting ##

An event's dispatch cycle can be interrupted prematurely if necessary. There are two ways of accomplishing this.

### Stopping Propagation ###

The `stop()` method will immediately stop the cycle at the current observer, discarding all upcoming 
observers from being notified. This can be triggered from the event object.

```hack
$observer = ($event) ==> {
    $event->stop();
};
```

Secondly, by returning a non-true/null/void from the observer callback, a stop will occur. 
Using this return format will also set the state. Jump to the next section for more information on states.

```hack
$observer = ($event) ==> {
    return false;
};
```

### Saving State ###

Every event object has a state that persists throughout the entire dispatch cycle. The state is usually a boolean `true`, 
which equates to a successful cycle, but could also be a `false` for an unsuccessful cycle, or simply a set of data 
to abort with.

To alter the state of an event, use `setState()` within the observer callback.

```hack
$observer = ($event) ==> {
    // Generate some data
    
    $event->setState($data);
};
```

The `getState()` method will retrieve the state once the cycle is complete.
