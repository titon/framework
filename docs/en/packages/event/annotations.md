# Annotations #

The following annotations are available in the event package.

### Observer(`string $event`[, `int $priority`[, `bool $once`]]) ###

When declared, the `Observer` annotation will automatically subscribe a class method as an observer to a specific event. The annotation accepts three arguments, the first being the event key, the second being the [priority level](emitting.md#priority-queue) (optional), and the last being the [once flag](emitting.md#one-time-notifications) (optional).

The class in which the annotations are declared must implement the `Titon\Event\Subject` interface for this to work correctly. The annotations should only be declared on methods.

```hack
class Foo implements Titon\Event\Subject {
    use Titon\Event\EmitsEvents;

    <<Observer('init')>>
    public function doAction(Event $event): mixed {
        // ...
    }
}
```

Once the annotations are declared, we can [wire them in the constructor](../annotation/wiring.md), which in turn triggers the automatic subscribing.

```hack
class Foo implements Titon\Event\Subject {
    use Titon\Event\EmitsEvents, Titon\Annotation\WiresAnnotations;

    public function __construct() {
        $this->wireAnnotations();
    }

    // ...
}
```

For a better understanding of this annotation, here are a few declaration examples.

```hack
<<Observer('init')>>
<<Observer('init', 15)>>
<<Observer('init', 25, true)>>
```
