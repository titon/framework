# Event #

Provides an implementation of the observer pattern, which permits listeners to subscribe to events.
When an event is dispatched by an emitter, it will cycle through and notify every observer.
Event propagation can be stopped by modifying the event object that is passed between observers.

```hack
$emitter = new Titon\Event\Emitter();
$emitter->register('db.save', inst_meth($object, 'method');
$emitter->registerListener(new Foo\Bar\CustomListener());
$emitter->emit('db.save', [&$data]);
```

Creating a `Listener` is easily accomplished by implementing an interface.

```hack
namespace Foo\Bar;

use Titon\Event\Event;
use Titon\Event\Listener;
use Titon\Event\ListenerMap;

class CustomListener implements Listener {
    public function save(Event $event, array<mixed> &$data): void {
        // ...
    }

    public function registerEvents(): ListenerMap {
        return Map {'db.save' => 'save'};
    }
}
```