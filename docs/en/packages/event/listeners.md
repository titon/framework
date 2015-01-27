# Listeners #

A `Titon\Event\Listener` is an interface that defines a mapping of observers that will be subscribed to events.
To utilize this functionality, implement the `subscribeToEvents()` method.

```hack
class ExampleListener implements Titon\Event\Listener {
    public function subscribeToEvents(): Titon\Event\ListenerMap {
        return Map {};
    }
}
```

This method should return a map that relates observers (the value) to an event (the key). Each observer 
declaration should reference a method in the current class. Furthermore, the declaration supports both a single observer, 
or multiple observers through a vector.

```hack
use Titon\Event\Event;

class ExampleListener implements Titon\Event\Listener {
    public function subscribeToEvents(): Titon\Event\ListenerMap {
        return Map {
            'init' => 'onInit',
            'save' => Vector {'doProcess, 'doCalculation'}
        };
    }
    
    public function onInit(Event $event): mixed {
        // ...
    }
    
    public function doProcess(Event $event): mixed {
        // ...
    }
    
    public function doCalculation(Event $event): mixed {
        // ...
    }
}
```

To pass a [priority level](emitting.md#priority-queue) or a [once flag](emitting.md#one-time-notifications), 
a map of key-values can be passed in place of the method name.

```hack
public function subscribeToEvents(): Titon\Event\ListenerMap {
    return Map {
        'init' => Map {'method' => 'onInit', 'once' => true}
        'save' => Vector {
            'doProcess, 
            Map {'method' => 'doCalculation', 'priority' => 25}
        }
    };
}
```

[Jump to the emitter docs for information on subscribing a listener.](emitting.md#subscribing-to-events)
