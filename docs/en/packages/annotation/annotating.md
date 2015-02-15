# Annotating #

Annotations are a small pieces of metadata declared on classes and methods. They make use of built-in [Hack attributes](http://docs.hhvm.com/manual/en/hack.attributes.php) and are represented by `<<` and `>>` with optional argument support. In the context of Titon, annotations take attributes a step further by providing annotation classes that map to attributes in a one-to-one relationship. These annotation classes provide constructor arguments with type hints, helper methods, and [automatic wiring](wiring.md) -- which is a form of bootstrapping.

As awesome as attribute are, there are a few caveats when using them, they are:

* Only primitive and array types can be used as arguments
* Only 1 attribute of the same name can be used per class or method

## Creating Annotations ##

An annotation is either built on or makes use of the `Titon\Annotation\Annotation` class. This class provides support for annotation mapping and instantiation through the factory system. To create a custom annotation, create a class that extends the `Annotation` class.

For our example, we'll create an annotation that aliases the current class with a new class. Any arguments passed to the `<<Alias>>` attribute will be passed to the annotations constructor.

```hack
class Alias extends Titon\Annotation\Annotation {
    protected string $alias;

    // Triggered when the annotation is first read
    public function __construct(string $alias) {
        $this->alias = $alias;
    }
}
```

<div class="notice is-info">
    Although this example is interesting, it's not practical in the real world as the class would have to be instantiated first for the alias to be created. It's merely for fun.
</div>

Once the annotation classes are created, we can map them in the annotation registry and make them available to the reader. The `Titon\Annotation\Registry::map()` method accepts a unique name, which is the declaration name, and the fully qualified class name.

```hack
Titon\Annotation\Registry::map('Alias', 'Some\Name\Space\Alias');
```

Now that annotations have been mapped, we can declare the `<<Alias>>` attribute on the class.

```hack
<<Alias('Bar')>>
class Foo { }
```

For the class alias to be defined, we must wire the annotation. Information on this can be found in the [wiring documentation](wiring.md).

## Reading Annotations ##

With annotations being declared, we can now read them and make use of their metadata using `Titon\Annotation\Reader`. The reader requires either a class name or an object.

```hack
$reader = new Titon\Annotation\Reader($class);
```

To extract all annotations on a class, use `getClassAnnotations()`, and on a specific method, use `getMethodAnnotations()`. Both of these methods will return a map with the key being the annotation name and the value being the annotation object.

```hack
$classAnnos = $reader->getClassAnnotations();
$methodAnnos = $reader->getMethodAnnotations('methodName');
```

To extract a single annotation on a class, use `getClassAnnotation()`, and on a specific method, use `getMethodAnnotation()`. Both of these methods will throw a `Titon\Annotation\Exception\MissingAnnotationException` if the annotation does not exist.

```hack
$classAnno = $reader->getClassAnnotation('Alias');
$methodAnno = $reader->getMethodAnnotation('methodName', 'Alias');
```

In some situations, it may be easier to extract annotations from all methods in a class. This can be accomplished using the `getAnnotatedMethods()` method, which returns a mapping of method names to annotation maps (the maps previously talked about).

```hack
$allMethodAnnos = $reader->getAnnotatedMethods();
```
