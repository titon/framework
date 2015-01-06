# Annotations #

Annotations allow metadata to be defined on classes and methods using [Hack attributes](http://docs.hhvm.com/manual/en/hack.attributes.php).
To make use of annotations, a class must implement the `Titon\Common\Annotator` interface and inherit the `Titon\Common\Annotateable` trait.

```hack
class Foo implements Titon\Common\Annotator {
    use Titon\Common\Annotateable;
}
```

## Defining Annotations ##

Annotations are defined either above the class name, or above the method name, using the format `<<Name>>`. 
Arguments can be passed to an annotation by wrapping values in parenthesis, `<<Name(Arg1, Arg2, ...)>>`. 
Multiple annotations can also be defined by separating annotations with a comma, `<<Name1, Name2, ...>>`. 
Here's an example of annotations in action.

```hack
<<Foo>>
class ExampleClass implements Titon\Common\Annotator {
    use Titon\Common\Annotateable;
    
    <<Bar(123), Baz(['key' => 'value'])>>
    public function exampleMethod(): void {}
}
```

## Loading Annotations ##

Once annotations have been defined, we need to load the annotations. This can be accomplished with the following methods 
inherited from the `Titon\Common\Annotator` interface.

<table class="table is-striped">
    <thead>
        <tr>
            <th>Method</th>
            <th>Description</th>
        </tr>
    </thead>
    <tbody>
        <tr>
            <td>getAnnotatedMethods(): Map&lt;string, Titon\Common\AnnotationMap&gt;</td>
            <td>Return a map of methods that have annotations.</td>
        </tr>
        <tr>
            <td>getClassAnnotation(string $name): Titon\Common\ArgumentList</td>
            <td>Return the arguments for a class annotation defined by name.</td>
        </tr>
        <tr>
            <td>getClassAnnotations(): Titon\Common\AnnotationMap</td>
            <td>Return a map of all annotations for the class. The key is the annotation name, and the value is the arguments.</td>
        </tr>
        <tr>
            <td>getMethodAnnotation(string $method, string $name): Titon\Common\ArgumentList</td>
            <td>Return the arguments for a method annotation defined by name.</td>
        </tr>
        <tr>
            <td>getMethodAnnotations(string $method): Titon\Common\AnnotationMap</td>
            <td>Return a map of all annotations for a method. The key is the annotation name, and the value is the arguments.</td>
        </tr>
    </tbody>
</table>

When calling `getClassAnnotation()` or `getMethodAnnotation()` with a name that does not exist, then a 
`Titon\Common\Exception\MissingAnnotationException` is thrown.

## Wiring Annotations ##

The last step in setting up annotations is to wire them up in the constructor. In layman's terms, this basically 
sets up any functionality that an annotation is supposed to provide. Using the example above, let's wire up the `Foo`, 
`Bar`, and `Baz` annotations.

```hack
<<Foo>>
class ExampleClass implements Titon\Common\Annotator {
    use Titon\Common\Annotateable;
    
    public function __construct() {
        $this->wireAnnotations();
    }
    
    <<Bar(123), Baz(['key' => 'value'])>>
    public function exampleMethod(): void {}
    
    private function wireAnnotations(): void {
        foreach ($this->getClassAnnotations() as $name => $args) {
            if ($name === 'Foo') {
                // ...
            }
        }
        
        foreach ($this->getMethodAnnotations('exampleMethod') as $name => $args) {
            if ($name === 'Bar') {
                // ...
            } else if ($name === 'Baz') {
                // ...
            }
        }
    }
}
```