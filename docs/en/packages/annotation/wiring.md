# Wiring #

Wiring is a concept of bootstrapping an annotation when the class it's declared on is instantiated. This wiring process allows for annotations to fully initialize itself based on the class and or method in which it was declared. Wiring will *only* happen on a class by class basis and is not triggered when [reading annotations](annotating.md#reading-annotations).

## Making An Annotation Wireable ##

To further explain this concept, let's make use of our [`Alias` annotation from the annotating documentation](annotating.md). Currently, the `Alias` annotation will *not* be wired when a class is instantiated. To rectify this, simply add the `Titon\Annotation\Wireable` interface to the annotation. This interface defines a `wire()` method which accepts an instance of the class and an optional method name. Let's update our example.

```hack
class Alias extends Titon\Annotation\Annotation implements Titon\Annotation\Wireable {
    protected string $alias;

    // Triggered when the annotation is first read
    public function __construct(string $alias) {
        $this->alias = $alias;
    }

    // Triggered when the annotation is wired
    public function wire<T>(T $class, string $method = ''): this {
        class_alias(get_class($class), $this->alias);
    }
}
```

<div class="notice is-info">
    The <code>T</code> type generics are required when using <code>wire()</code>.
</div>

In our example above, the `wire()` method receives an instance of the class the annotation is declared in, and a method name if it was declared on a method. We can now use this information to define our alias -- success!

However, to actually trigger the `wire()` method, we must wire the annotation, which will be explained next.

## Wiring An Annotation ##

To wire an annotation, we can implement the `Titon\Annotation\WiresAnnotations` trait, which internally handles the entire read and wire process automatically. This trait provides 5 methods, `wireAnnotations()` for wiring all class and method annotations, `wireClassAnnotations()` for wiring all class annotations, `wireMethodAnnotations()` for wiring all annotations on a method, and finally, `wireClassAnnotation()` and `wireMethodAnnotation()` for wiring a single annotation by name. All of these methods should be triggered within the parent classes constructor.

Continuing with our `Alias` example, let's finalize the `Foo` class by wiring the annotation in the constructor.

```hack
<<Alias('Bar')>>
class Foo {
    use Titon\Annotation\WiresAnnotations;

    public function __construct() {
        $this->wireClassAnnotation('Alias');
    }
}
```

And that's it! The `Alias` annotation will now be bootstrapped any time the class is instantiated.
