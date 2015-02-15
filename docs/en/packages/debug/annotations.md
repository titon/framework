# Annotations #

The following annotations are available in the debug package.

### Deprecated(`string $message`) ###

When declared on a class, the `Deprecated` annotation marks a class as deprecated and will log a notice, and a custom message, anytime the class is instantiated. The logger defined in `Titon\Debug\Debugger` will be used for logging.

To mark a class as deprecated, declare the annotation, and then [wire it up in the constructor](../annotation/wiring.md).

```hack
<<Deprecated('Please use Bar instead.')>>
class Foo {
    use Titon\Annotation\WiresAnnotations;

    public function __construct() {
        $this->wireClassAnnotation('Deprecated');
    }
}
```

### Monitor([`string $callback`]) ###

Similar to the `Deprecated` annotation, the `Monitor` annotation will log an informational message anytime the class is instantiated. This message will include a file path and line number for the location in which the instantiation occurred. 

```hack
<<Monitor>>
class Foo {
    use Titon\Annotation\WiresAnnotations;

    public function __construct() {
        $this->wireClassAnnotation('Monitor');
    }
}
```

Furthermore, the `Monitor` annotation accepts an optional callback that will be triggered when the annotation is wired. This allows for userland code to hook into the monitoring process. The callback can either be a function, or a static class method.

```hack
<<Monitor('global_function')>>
<<Monitor('Foo::staticMethod')>>
```
